/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 10:30:56 by bahommer          #+#    #+#             */
/*   Updated: 2024/02/12 11:30:52 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HPP
# define PARSING_HPP

#include "webserv.hpp"
#include "../parsing/Server.hpp"

# define PARAM 8
# define MAX_CO 10
# define MAXLINE 4096

class Server;
class HttpRequest;
class HttpRequestAllow;

/*readConfigFile.cpp*/
void readConfigFile (std::vector<Server> & servers, char const* file);

/*allocateSTL.cpp*/
void	allocatePollFds(std::vector<Server> const& servers,
	std::vector<struct pollfd> & pollfds);
void	allocateServersMap(std::vector<Server> & servers, std::map<int, Server*>  & serversMap); 

/*handlePoll.cpp*/
void handlePollin(std::vector< struct pollfd> & pollfds, std::map<int, std::pair<struct sockaddr_in, HttpRequest*> >  & clientMap, std::vector<Server>  & servers, HttpRequestAllow & check, size_t i);
void handlePollout(std::vector< struct pollfd> & pollfds, std::map<int, std::pair<struct sockaddr_in, HttpRequest*> > & clientMap, size_t i);
void handlePollerr(std::vector< struct pollfd> & pollfds, std::map<int, std::pair<struct sockaddr_in, HttpRequest*> > & clientMap, size_t i);
#endif
