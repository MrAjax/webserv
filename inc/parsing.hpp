/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 10:30:56 by bahommer          #+#    #+#             */
/*   Updated: 2024/02/05 10:31:27 by bahommer         ###   ########.fr       */
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

/*readConfigFile.cpp*/
void readConfigFile (std::vector<Server> & servers, char const* file);

/*allocateSTL.cpp*/
void	allocatePollFds(std::vector<Server> const& servers,
	std::vector<struct pollfd> & pollfds);
void	allocateServersMap(std::vector<Server> & servers, std::map<int, Server*>  & serversMap); 
#endif
