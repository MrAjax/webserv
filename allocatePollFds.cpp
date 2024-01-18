/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   allocatePollFds.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 12:53:35 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/18 16:06:23 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp"
#include "Server.hpp"

void	allocatePollFds(std::vector<Server> const& servers,
	std::vector<struct pollfd> & pollfds,
		std::map<struct pollfd*, Server const*, pollFdComparer> & fdsMap)
{
	
	for (size_t i = 0; i < servers.size() ; i++) 
	{
		struct pollfd temp;

		temp.fd = servers[i].getSocketfd();
		std::cout << "getSocketfd = " << temp.fd << std::cout;
		temp.events = POLLIN;
		temp.revents = 0;

		pollfds.push_back(temp);
		fdsMap.insert(std::make_pair(&pollfds[i], &servers[i]));
	}
}
