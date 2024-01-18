/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 09:22:50 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/18 16:05:04 by bahommer         ###   ########.fr       */
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
	std::map<struct pollfd*, Server const*, pollFdComparer> fdsMap;
	if (readConfigFile(servers, av[1]) == -1)
		return 1;
	allocatePollFds(servers, pollfds, fdsMap);

	for (std::map<struct pollfd*, Server const*, pollFdComparer>::iterator it = fdsMap.begin(); it != fdsMap.end(); ++it) 
	{
		std::cout << "fds = " << it->first->fd << std::endl;
	}	

	return 0;
}
