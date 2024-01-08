/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 09:22:50 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/08 10:39:40 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp" 

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
	return 0;
}
