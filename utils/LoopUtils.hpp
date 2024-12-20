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

int	send_response_to_client(int connfd, std::string response, HttpRequest &Req);
void	send_response(int connfd, Server &serv ,HttpRequest &Req);

#endif
