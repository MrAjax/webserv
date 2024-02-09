#ifndef MAINLOOP_HPP
#define MAINLOOP_HPP

#include "../inc/webserv.hpp"
#include "../parsing/Server.hpp"
#include "../request/HttpRequest.hpp"
#include "../response/HttpResponse.hpp"

#include "ContentTypeFinder.hpp"
#include "HttpStatusCode.hpp"


void    removeTimeout(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds);
bool    killRequest(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds, std::size_t i);

void    addingNewClient(HttpRequest **clientRequest, struct sockaddr_in &clientAddr, 
    std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds);

bool    isListener(int fd, std::vector<Server> servers);

void	removeRequest(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds, std::vector<Server> servers);

void	exitClean(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds);

#endif
