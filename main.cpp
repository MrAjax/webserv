/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 09:22:50 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/16 10:05:01 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp" 
#include "serv_config.hpp"

int	main(int ac, char **av)
{
	if (ac != 2) {
		std::cerr << "Usage ./webserv config_file!" << std::endl;
		return 0;
	}
	int fd = open(av[1], O_RDONLY);

	if (fd == -1) {
		std::cerr << "Error while opening config file: " << strerror(errno) << std::endl;
		return 0;
	}
	close(fd);

	try {
		serv_config servers(av[1]); // create every server 
	} catch (std::exception& e) {
		std::cerr << " "  << e.what() << std::endl;
	}
	return 0;
}
