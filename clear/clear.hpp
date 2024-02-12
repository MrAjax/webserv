#ifndef CLEAR_HPP
#define CLEAR_HPP

#include "../inc/webserv.hpp"
#include "../parsing/Server.hpp"
#include "../request/HttpRequest.hpp"
#include "../utils/LoopUtils.hpp"

void	removeRequest(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap,
            std::vector<struct pollfd> &pollfds, std::vector<Server> servers);
bool    killRequest(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap,
            std::vector<struct pollfd> &pollfds, std::size_t i);

void	exitClean(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds);

void    removeTimeout(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds);

#endif
