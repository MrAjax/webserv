/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readConfigFile.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 12:43:31 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/23 13:53:21 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/parsing.hpp"

void trimWhiteSpaces(std::string & str) {

	size_t first = str.find_first_not_of(" \t\n\r\f\v");
	if (first != std::string::npos) {
		str.erase(0, first);
	}		

	size_t last = str.find_last_not_of(" \t\n\r\f\v");
	if (last != std::string::npos) {
		str.erase(last + 1);
	}	
}	

void countBrackets(int & bracket, std::string const& line) 
{
	if (line.find("{") != std::string::npos) 
		bracket++;
	if (line.find("}") != std::string::npos)
		bracket--;
	if (bracket < 0)
		throw std::runtime_error("Miss bracket, config file bad syntax");
}

void chekLastchar(std::string & line) {

	if (line[line.length() - 1] == '{' || line[line.length() - 1] == '}')
		return;
	if (line[line.length() - 1] != ';')
	 	throw std::runtime_error("line [" + line + "] Miss ; in the end");
	line.erase(line.length() - 1); //suppr ;
	trimWhiteSpaces(line);
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
		int	bracket = 0;
	
		while (getline(ifs, line)) {

			if (line.compare(0, 6, "server") == 0) { //line MUST start with server, no \t or " "
				if (bracket != 0)
					throw std::runtime_error("Unclosed bracket");
				if (line.find("{") != std::string::npos) { //{ MUST be on the same line 
					recording = true;
					block.push_back(line);
				}	
				else
					throw std::runtime_error("Miss bracket in the beginning of server");
			}
			trimWhiteSpaces(line);
			if (line[0] == '\0' || line[0] == '#') // skip empty and comment lines
				continue;
			chekLastchar(line);	
			std::cout << "line recorded = [" << line << "]" << std::endl;
			if (recording == false && line.find_first_not_of(" \t\n\r\f\v") != std::string::npos) 
				throw std::runtime_error("unknown directive \"" + line + "\"");
			countBrackets(bracket, line);
			if (line[0] == '}' && bracket == 0 && recording == true) {
				block.push_back(line);
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
		return -1;
	}
	return 0;
}
