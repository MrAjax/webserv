/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   allocateSTL.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 12:53:35 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/23 13:51:26 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/parsing.hpp"
#include "Server.hpp"

void	allocatePollFds(std::vector<Server> const& servers,
	std::vector<struct pollfd> & pollfds)
{
	for (size_t i = 0; i < servers.size() ; i++) 
	{
		struct pollfd temp;
		std::vector<struct pollfd>::iterator it = pollfds.begin();

		while (it != pollfds.end()) //check socketfd duplicate
		{
			if (it->fd == servers[i].getSocketfd())
				break;
			it++;	
		}
		if (it == pollfds.end())
		{
			temp.fd = servers[i].getSocketfd();
			temp.events = POLLIN;
			temp.revents = 0;
			pollfds.push_back(temp);
		}
	}
}	

void	allocateServersMap(std::vector<Server> & servers, std::map<int, Server*> & serversMap) 
{
	for (size_t i = 0; i < servers.size(); i++) 
	{
		serversMap[servers[i].getSocketfd()] = &servers[i];
	}
}
