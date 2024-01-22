/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readConfigFile.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 12:43:31 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/22 12:47:27 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp"

void trimWhiteSpaces(std::string & str) {

	std::size_t first = str.find_first_not_of(" \t\n\r\f\v");
	if (first != std::string::npos) {
		str.erase(0, first);
	}		
}	


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
			trimWhiteSpaces(line);
			if (line[0] == '\0' || line[0] == '#') // skip empty and comment lines
				continue;
	/*		else if (recording == false && line.find("server") == std::string::npos) { // Text uncomment outside a bloc server 
				throw std::runtime_error("*/
			else if (line.find("server {") != std::string::npos) {
				std::cout << "line trim= " << line << std::endl;
				recording = true;
			}
			else if (line[0] == '}' && recording == true) {
				std::cout << "line trim= " << line << std::endl;
				servers.push_back(Server(block, servers, i));
				block.clear();
				recording = false;
				i++;
			}
			else if (recording == true) {
				std::cout << "line trim= " << line << std::endl;
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
