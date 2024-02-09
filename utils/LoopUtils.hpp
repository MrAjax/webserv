#ifndef LOOPUTILS_HPP
#define LOOPUTILS_HPP

#include "../inc/webserv.hpp"

#include "../response/HttpResponse.hpp"
#include "../signal/signal.hpp"

#include "ContentTypeFinder.hpp"
#include "HttpStatusCode.hpp"

void	init_log_file(void);
void	init_server(void);

bool    isListener(int fd, std::vector<Server> servers); 

void    addingNewClient(HttpRequest **clientRequest, struct sockaddr_in &clientAddr, 
    std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds);

void	send_response_to_client(int connfd, std::string response);
void	send_response(int connfd, Server &serv ,HttpRequest &Req);

#endif
