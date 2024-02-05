#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "inc/webserv.hpp"
#include "signal/signal.hpp"
#include "inc/parsing.hpp"
#include "utils/MainLoop.hpp"

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

static	void	send_response(int connfd, Server &serv ,HttpRequest &Req) {

	std::string	response;
	server_log("Activity detected on server: " + serv.getServerName(), DEBUG);
	if (connfd < 0)
		throw error_throw("send response - main.cpp", true);

	
	
	try {
		if (Req.getMyserver() == NULL)
			throw StatusSender::send_status(Req.getStatusCode(), serv); //Pb si pas de servers

		server_log("Parsing Request...", DEBUG);
		std::string	request_header = Req.getHeaderRequest();

		server_log("All the chunks received", DEBUG);
		if (request_header.empty()) {
			server_log("Invalid request", ERROR);
			throw std::runtime_error(std::string(REDD) + "Unsupported request type" + std::string(ENDD));
		}
		server_log("Request is valid", DEBUG);
		server_log(request_header + "\n\n", DIALOG);
		server_log(Req.getBodyRequest() + "\n\n", DIALOG);
		server_log("Building Response...", DEBUG);

		HttpResponse	Rep(Req, serv);

		
		response = Rep.get_response(serv);
		server_log(Rep.get_header(), DIALOG);
		send_response_to_client(connfd, response);
	}
	catch (std::string &s) {
		response = s;
		send_response_to_client(connfd, response);
	}
	// Remove the client in case of an error
}

static	void	init_log_file() {
	std::fstream log_file(LOG_FILE, std::ios::out | std::ios::trunc);
    
    if (!log_file.is_open())
		throw error_throw("log file cannot be created - init_log_file - main.cpp", true);
    log_file.close();
}

void	init_server(void) {
	g_sig = 0;
	signal(SIGINT, &sigint_handler);
	signal(SIGQUIT, &sigquit_handler);
	init_log_file();
	server_log("SERVER STARTED", INFO);
}

bool isListener(int fd, std::vector<Server> servers) {
	for (size_t i = 0; i < servers.size(); i++) {
		if (fd == servers[i].getSocketfd())
			return true;	
	}
	return false;
}

#define MAX_NUMBER_REQUEST 100

int main(int ac, char **av)
{
	if (ac != 2) {
		std::cerr << "Usage ./webserv config_file!" << std::endl;
		return 0;
	}

		std::vector<Server>			servers;
		std::vector<struct pollfd>	pollfds;
		std::map<int, Server*>		serversMap;
		std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > clientMap;
		std::string					dot[3] = {".  ", ".. ", "..."};
		int							n = 0;

	try {
		init_server();
		readConfigFile(servers, av[1]);
		allocatePollFds(servers, pollfds); //set struct pollfd
		allocateServersMap(servers, serversMap); //set map pollfd.fd Server*
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		for (size_t i = 0; i < servers.size(); i++) {
			if (servers[i].getSocketfd() != -1)
				close(servers[i].getSocketfd());
		}	
		return 1;
	}
	try {
		
		while (g_sig == 0) { /* Here is the main loop */
			std::size_t sizePollfds = pollfds.size();
		
			if (poll(&pollfds[0], sizePollfds, 1000) == -1)
				std::cerr << "poll error: " << strerror(errno) << std::endl;

	//		std::cout << YELLOW "Number of pollfd= " RESET << sizePollfds << std::endl;			
			if (n == 3)
				n = 0;
			std::cout << "\rWebserv is running" << dot[n++] << std::flush;

			for(size_t i = 0; i < sizePollfds; i++)
			{
				if (pollfds[i].revents & POLLIN) //EVENT!
				{
					server_log("EVENT", DEBUG);
					std::map<int, Server*>::iterator it = serversMap.find(pollfds[i].fd); 
					if (it != serversMap.end()) {
						if (isListener(pollfds[i].fd, servers)) //socketfd is listener == 1st co
						{
			//				std::cout << "server name: " << it->second->getServerName() << "\n";
							struct sockaddr_in clientAddr;
							socklen_t tempAddrlen = sizeof(clientAddr);

					//		std::cout << PURPLE "New connexion with listener socket fd= " RESET <<  pollfds[i].fd << std::endl;
							int clientFd = accept(pollfds[i].fd, (struct sockaddr *)&clientAddr, &tempAddrlen); 
					//		std::cout << PURPLE "New client fd= " RESET << clientFd << std::endl;

							if (clientFd == -1) {
								std::cerr << "Accept error: " << strerror(errno) << std::endl;
							}
							else {
								HttpRequest *clientRequest = new HttpRequest(clientFd, servers, pollfds[i].fd);
								if (sizePollfds > MAX_NUMBER_REQUEST)
									clientRequest->setStatusCode(429);
								addingNewClient(&clientRequest, clientAddr, serversMap, it, clientMap, pollfds);
							}
						}
						else // socketfd aldready set c/p from HttpRequest
						{
					//		std::cout << GREEN "pollfds.fd= " RESET << pollfds[i].fd << std::endl;
					//		std::cout << GREEN "requestfd.fd= " RESET << clientMap[pollfds[i].fd].second->getConnfd() << std::endl;

							//std::cout << "server name: " << it->second->getServerName() << "\n";
							server_log("other request on clientFD", DEBUG);
							int status = clientMap[pollfds[i].fd].second->processingRequest();
							// clientMap[pollfds[i].fd].second->printAttribute();
							if (status > 200)
							{
								send_response(pollfds[i].fd, *it->second, *clientMap[pollfds[i].fd].second);
								clientMap[pollfds[i].fd].second->resetRequest();
							}
							else if (status == KILL_ME)
								killRequest(clientMap, pollfds, i);
						}
					}
				}
				removeTimeout(clientMap, pollfds);
			}
		}
	}		
	catch (const std::exception &e) {
		std::cerr << e.what() << "\n";
	}
	for (size_t i = 0; i < pollfds.size(); i++) {
		close(pollfds[i].fd);
	}	
}
