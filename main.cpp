/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 09:22:50 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/16 13:24:20 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp" 
#include "Server.hpp"

int	main(int ac, char **av)
{
	if (ac != 2) {
		std::cerr << "Usage ./webserv config_file!" << std::endl;
		return 0;
	}

	std::ifstream ifs;

	ifs.open(av[1]);
	if (!ifs) {
		std::cerr << "Error while opening config file." << std::endl;
		return 0;
	}
	try {
		std::vector<Server> servers;
		std::string line;
		std::vector<std::string> block;
		bool recording = false;
		int	i = 0;
	
		while (getline(ifs, line)) {
			if (line.find("server {") != std::string::npos) {
				recording = true;
			}
			else if (line[0] == '}' && recording == true) {
				servers.push_back(Server(block, servers, i));
				block.clear();
				recording = false;
				i++;
			}
			else if (recording == true) {
				block.push_back(line);
			}
		}
		ifs.close();
	} catch (std::exception& e) {
		std::cerr << " "  << e.what() << std::endl;
		ifs.close();
	}
	return 0;
}
