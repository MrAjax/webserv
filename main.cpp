#include "request/HttpRequest.hpp"
#include "request/HttpRequestAllow.hpp"
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
	try {
		if (Req.getMyserver() == NULL)
		{
			std::cout << RED "NO SERVER" RESET << std::endl;
			return ;
		}
		server_log("Activity detected on server: " + serv.getServerName(), DEBUG);
		if (connfd < 0)
			throw error_throw("send response - main.cpp", true);

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

		HttpRequestAllow check(50, 1000);
	
	try {
		init_server();
		readConfigFile(servers, av[1]);
		allocatePollFds(servers, pollfds); //set struct pollfd
		// allocateServersMap(servers, serversMap); //set map pollfd.fd Server*  NE sert plus
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

			if (n == 3)
				n = 0;
			std::cout << "\rWebserv is running" << dot[n++] << std::flush;
			for(size_t i = 0; i < sizePollfds; i++)
			{
				if (pollfds[i].revents & POLLIN) //EVENT!
				{
					server_log("EVENT", DEBUG);
					if (isListener(pollfds[i].fd, servers)) //socketfd is listener == 1st co
					{
						struct sockaddr_in clientAddr;
						socklen_t tempAddrlen = sizeof(clientAddr);
						int clientFd = accept(pollfds[i].fd, (struct sockaddr *)&clientAddr, &tempAddrlen); 
						if (clientFd == -1) {
							server_log(std::string(REDD) + "Accept error for socketfd " + int_to_str(pollfds[i].fd), ERROR);
						}
						else {
							try
							{
								HttpRequest *clientRequest = new HttpRequest(clientFd, servers, pollfds[i].fd);
								check.allowRequest(pollfds, *clientRequest);
								addingNewClient(&clientRequest, clientAddr, clientMap, pollfds);
							}
							catch (const std::exception &e)
							{
								server_log(std::string(REDD) + "HttpRequest allocation fail clientfd " + int_to_str(clientFd), ERROR);
							}
						}
					}
					else // socketfd aldready set c/p from HttpRequest
					{
						server_log("Processing request clientFd " + int_to_str(pollfds[i].fd), DEBUG);
						
						if (clientMap[pollfds[i].fd].second->processingRequest() >= 200)
						{
							// clientMap[pollfds[i].fd].second->printAttribute();
							pollfds[i].events = POLLOUT;
						}
						// else if (clientMap[pollfds[i].fd].second->getStatusCode() == KILL_ME)
							// killRequest(clientMap, pollfds, i);
					}
					
				}
				if (pollfds[i].revents & POLLOUT)
				{
					if (clientMap[pollfds[i].fd].second->getStatusCode() != KILL_ME)
						send_response(pollfds[i].fd, *clientMap[pollfds[i].fd].second->getMyserver(), *clientMap[pollfds[i].fd].second); // get my server peut etre = NULL risque segFault
					if (clientMap[pollfds[i].fd].second->getStatusCode() >= 400 || clientMap[pollfds[i].fd].second->getStatusCode() == KILL_ME)
					{
						std::cout << RED "KILLREQUEST\n";
						// killRequest(clientMap, pollfds, i);
					}
					else
					{
						clientMap[pollfds[i].fd].second->resetRequest();
						pollfds[i].events = POLLIN;

					}
				}
			}
			removeRequest(clientMap, pollfds, servers);
			removeTimeout(clientMap, pollfds);
		}
	}		
	catch (const std::exception &e) {
		std::cerr << e.what() << "\n";
	}
	for (size_t i = 0; i < pollfds.size(); i++) {
		close(pollfds[i].fd);
	}
	for (std::map<int, std::pair<struct sockaddr_in, HttpRequest* > >::iterator it =  clientMap.begin();
		it != clientMap.end(); it++)
	{
		if (it->second.second != NULL)
		{
			delete it->second.second;
			it->second.second = NULL;
		}
	}
}
