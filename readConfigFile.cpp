/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readConfigFile.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 12:43:31 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/18 12:49:46 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp"

int readConfigFile (std::vector<Server> & servers, char const* file)
{
	std::ifstream ifs;

	ifs.open(file);
	if (!ifs) {
		std::cerr << "Error while opening config file." << std::endl;
		return -1;
	}
	try {
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