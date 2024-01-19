/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 09:22:50 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/19 09:41:33 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp" 

int	main(int ac, char **av)
{
	if (ac != 2) {
		std::cerr << "Usage ./webserv config_file!" << std::endl;
		return 0;
	}

	std::vector<Server> servers;
	std::vector<struct pollfd> pollfds;
	std::map<int, Server*> serversMap;

	if (readConfigFile(servers, av[1]) == -1)
		return 1;
	allocatePollFds(servers, pollfds); //set struct pollfd
	allocateServersMap(servers, serversMap); //set map pollfd.fd Server*

/*
	std::map<int, Server*>::iterator it;
	
	for (it = serversMap.begin(); it != serversMap.end(); ++it) {
		std::cout << it->first << " " << it->second->getPort() << std::endl;
	}	*/

	for(;;) 
	{
		int poll_count = poll(&pollfds[0], pollfds.size(), -1);
		if (poll_count == -1) 
			std::cerr << "poll error: " << strerror(errno) << std::endl;
	}
	
	return 0;
}
