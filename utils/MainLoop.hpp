#ifndef MAINLOOP_HPP
#define MAINLOOP_HPP

#include "../inc/webserv.hpp"
#include "../request/HttpRequest.hpp"
#include "../response/HttpResponse.hpp"

#include "ContentTypeFinder.hpp"
#include "HttpStatusCode.hpp"


void    removeTimeout(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds);
void    killRequest(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds, int i);

void    addingNewClient(int &clientFd, struct sockaddr_in &clientAddr, 
    std::map<int, Server*> &serversMap, std::map<int, Server*>::iterator &it,
    std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds);

#endif
