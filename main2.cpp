#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "inc/webserv.hpp"
#include "signal/signal.hpp"
#include "inc/parsing.hpp"

volatile int	g_sig;

/* 
	ERROR TO MANAGE IN THE REQUEST PARSER:

		error 405 - Method Not Allowed
	Si methode non connue -> error 405 - Method Not Allowed 

		408 - Request Timeout 
	Set chrono
	If timeout --> Send error 408 - Request Timeout 
	(Avoid the server to wait for an infinite time to process a request)

		411 - Length Required 
	If the field Content-Length is not set (for a post request i guess)
	then, send StatusCode 411 - Length Required

		413 - Payload Too Large
	If the request is too large to satisfy the server rules (very usefull for a post)
	then, send StatusCode 413 - Payload Too Large

		414 - URI Too Long

		415 - Unsuppported Media Type 
	(Use the Content Type Finder which returns an empty string in case of unsupported media type)
	
		429 - Too Many Requests
	Compter le nombre de requetes recues par client pour un labs de temps donne
	Definir une limite de requete par client sur un intervalle de temps a choisir
	Si trop de requests en meme temps (a la suite) pour un meme clent,
	alors send Status Code 429 - Too Many Requests

		431 - Request Header Fields Too Large 
	Verifier la taille du header, si header trop large --> Send 431 - Request Header Fields Too Large 

		501 - Not Implemented 
	Methode non supportee (PUT, HEAD, etc) --> Send 501 - Not Implemented 

		505 - HTTP Version Not Supported
	Si protocole different de HTTP/1.1 --> 505 - HTTP Version Not Supported
	
		Error 507 - Insufficient Storage
	Si pas assez de place pour stocker un POST --> Error 507 - Insufficient Storage

*/

static	void	send_response_to_client(int connfd, std::string response) {
	server_log("Sending response...", DEBUG);
	write(connfd, response.c_str(), response.length());
	server_log("Response sent", INFO);
}

static	void	send_response(HttpRequest &Request) {

	std::string	response;

	try {

		server_log("Parsing Request...", DEBUG);
		std::string	request_header = Request.getHeaderRequest();
		server_log("All the chunks received", DEBUG);
		if (request_header.empty()) {
			server_log("Invalid request", ERROR);
			throw std::runtime_error(std::string(REDD) + "Unsupported request type" + std::string(ENDD));
		}
		server_log("Request is valid", DEBUG);
		server_log(request_header + "\n\n", DIALOG);
		server_log("Building Response...", DEBUG);

		HttpResponse	Rep(Request);
		response = Rep.get_response();
		server_log(Rep.get_header(), DIALOG);
		send_response_to_client(Request.getConnfd(), response);
	}
	catch (std::string &s) {
		response = s;
		send_response_to_client(Request.getConnfd(), response);
	}
	// Remove the client in case of an error
}

static	void	init_log_file() {
	std::fstream log_file(LOG_FILE, std::ios::out | std::ios::trunc);
    
    if (!log_file.is_open())
		throw error_throw("log file cannot be created - init_log_file - main.cpp");
    log_file.close();
}

void	init_server(void) {
	g_sig = 0;
	signal(SIGINT, &sigint_handler);
	signal(SIGQUIT, &sigquit_handler);
	init_log_file();
	server_log("SERVER STARTED", INFO);
}
/*
void	stop_server(t_server &server) {
	close(server.listenfd);
	close(server.connfd);
	server_log("Connection closed", INFO);
	server_log("SERVER STOPPED", INFO);
	std::cout << std::string(GREENN) + "\nConnection closed - Bye!\n" + std::string(ENDD);
}
*/

int main(int ac, char **av)
{
	if (ac != 2) {
		std::cerr << "Usage ./webserv config_file!" << std::endl;
		return 0;
	}

	try {
		std::vector<Server> servers;

		std::vector<struct pollfd> pollfds;

		std::map<int, Server*> serversMap; 

		std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > clientMap;

		init_server();
		if (readConfigFile(servers, av[1]) == -1)
			return 1;
		allocatePollFds(servers, pollfds); //set struct pollfd -> on associe le socket du/des servers à des pollfd struct
		allocateServersMap(servers, serversMap); //set map pollfd.fd Server* -> on fait une map avec socket server = server en question


		while (g_sig == 0) { /* Here is the main loop */

			int poll_count = poll(&pollfds[0], pollfds.size(), -1); // très astucieux, mais marche pour de vrai ?
			if (poll_count == -1) 
				std::cerr << "poll error: " << strerror(errno) << std::endl;

			std::size_t pollSize = pollfds.size();
			std::cout << "POLLSIZE= " << pollSize << std::endl;
			for(size_t i = 0; i < pollSize; i++) //attention on ajoute des éléments à pollfds au fur et à mesure sans check poll
			{
				if (pollfds[i].revents & POLLIN) //EVENT!
				{
					server_log("EVENT", DEBUG);
					std::map<int, Server*>::iterator it = serversMap.find(pollfds[i].fd); //pour trouver si on a un listener
					if (it != serversMap.end()) //socketfd is listener == 1st co
					{
						struct sockaddr_in clientAddr;
						socklen_t tempAddrlen = sizeof(clientAddr);

						std::cout << PURPLE << pollfds[i].fd << " =listener New connexion processing\n" RESET;
						int clientFd = accept(pollfds[i].fd, (struct sockaddr *)&clientAddr, &tempAddrlen);
						std::cout << PURPLE << clientFd << " Acceptation connexion processing\n" RESET;
						
						if (clientFd == -1){
							std::cerr << "Accept error: " << strerror(errno) << std::endl;
						}
						else {
							HttpRequest *clientRequest = new HttpRequest(clientFd);
							clientMap[clientFd] = std::make_pair(clientAddr, clientRequest); //add client information to map Client
							struct pollfd newPfd;
							newPfd.fd = clientFd;
							newPfd.events = POLLIN;
							pollfds.push_back(newPfd); // add new fd to monitoring
							server_log("New connexion on fd " + int_to_str(clientFd) , DEBUG);
							// send_response(clientFd); //on doit d'abord check si on peut recv via poll je pense


						// 	std::cout << RED << clientFd << " Start processing\n" RESET;
						// 	clientRequest->processingRequest();
						// 	clientRequest->printAttribute();
						// 	std::cout << RED << clientFd << " End processing\n" RESET;

						// // if (status == DONE_ALL)
						// // {
						// 	send_response(*clientRequest);

						// 	delete clientRequest; //on delete les request qui ne servent plus du tout
						// 	clientMap.erase(clientFd);
						// 	pollfds.back().fd = -1;
						// 	// close(pollfds.back().fd);
						}	
					}
					else // socketfd aldready set c/p from HttpRequest
					{
						server_log("other request on clientFD", DEBUG);

						server_log("Activity detected", DEBUG);
						if (pollfds[i].fd < 0)
							throw error_throw("send response - main.cpp");

						if (pollfds[i].events == POLLOUT)
							std::cout << "POLLOUT HERE IN 1" << std::endl;

						std::cout << GREEN << pollfds[i].fd << " Start processing\n" RESET;
						clientMap[pollfds[i].fd].second->processingRequest();
						clientMap[pollfds[i].fd].second->printAttribute();
						std::cout << GREEN << pollfds[i].fd << " End processing\n" RESET;

						if (pollfds[i].events == POLLOUT)
							std::cout << "POLLOUT HERE IN 2" << std::endl;
						
						// if (status == DONE_ALL)
						// {
							send_response(*clientMap[pollfds[i].fd].second);

							delete clientMap[pollfds[i].fd].second; //on delete les request qui ne servent plus du tout
							clientMap.erase(pollfds[i].fd);
							std::cout << RED << pollfds[i].fd << " SUPPRESSION DE CE FD\n" RESET;
							pollfds[i].fd = -1;
							close(pollfds[i].fd);
						

						// }
					/*	u_int8_t recvline[MAXLINE + 1];
						memset(recvline, 0, MAXLINE);
						int numbytes;
						numbytes = recv(pollfds[i].fd, recvline, MAXLINE - 1, 0);
						if (numbytes == 0) { //connection close
							std::map<int, struct sockaddr_in>::iterator it;
							it = clientMap.find(pollfds[i].fd);
							close(pollfds[i].fd);
							clientMap.erase(it); //delete fd client side
							pollfds.erase(pollfds.begin() + i); //delete fd monitoring side
						}*/
					}
				}
			}
			for (std::size_t i = 0; i < pollfds.size(); i++)
			{
			// 	// std::map<int, std::pair<struct sockaddr_in, HttpRequest* > >::iterator it = clientMap.find(pollfds[i].fd);
			// 	// if (it != clientMap.end())
			// 	// {
			// 	// 	if (clientMap[pollfds[i].fd].second->checkTimeout())
			// 	// 	{
			// 	// 		delete clientMap[pollfds[i].fd].second;
			// 	// 		clientMap.erase(pollfds[i].fd);
			// 	// 		pollfds[i].fd = -1;
			// 	// 		close(pollfds[i].fd);
			// 	// 	}
			// 	// }
				if (pollfds[i].fd == -1)
				{
					pollfds.erase(pollfds.begin() + i);
				}
				
			}
		}
	}		
	catch (const std::exception &e) {
		std::cerr << e.what() << "\n";
	}
}
