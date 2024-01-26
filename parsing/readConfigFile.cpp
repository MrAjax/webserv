/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readConfigFile.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 12:43:31 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/26 12:43:02 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/parsing.hpp"
#include "../utils/utils.hpp"

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
	if (line.find("}") != std::string::npos) {
		if (line[0] != '}')
	 		throw error_throw("invalid parameter \"" + line + "\" - config file", false);
		else if (line.find_first_not_of(" \t\n\r\f\v", 1) != std::string::npos) 
			throw error_throw("unknown directive \"" + line + "\" - config file", false);
		bracket--;
	}	
	if (bracket < 0)
		throw error_throw("Miss closed bracket - config file", false);
}

void chekLastchar(std::string & line) {

	if (line[line.length() - 1] == '{' || line[line.length() - 1] == '}')
		return;
	if (line[line.length() - 1] != ';')
	 	throw error_throw("invalid parameter \"" + line + "\" - config file", false);
	line.erase(line.length() - 1); //suppr ';'
	trimWhiteSpaces(line);
}

void checkServerLine(std::string & line, int bracket) { //check first line "server {"

	if (bracket != 0)
		throw error_throw("Unclosed bracket - config file", false);
	size_t pos = 6;	
	while(pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	if (line[pos] == '\0')  
		throw error_throw("Miss bracket in the beginning of server - config file" , false);
	else if (line[pos] != '{')	
		throw error_throw("unknown directive \"" + line.substr(pos) + "\" - config file", false);
	else if (line[pos + 1] != '\0')	
		throw error_throw("unknown directive \"" + line.substr(pos + 1) + "\" - config file", false);
}		

void readConfigFile (std::vector<Server> & servers, char const* file)
{
	std::ifstream ifs;

	ifs.open(file);
	if (!ifs) {
		throw error_throw("Config file cannot be opened - readConfigFile - parsing/readConfigFile.cpp", true);
	}
	std::string line;
	std::vector<std::string> block;
	bool recording = false;
	int	i = 0;
	int	bracket = 0;
	
	while (getline(ifs, line)) {

		trimWhiteSpaces(line);
		if (line[0] == '\0' || line[0] == '#') // skip empty and comment lines
			continue;
		if (line.compare(0, 6, "server") == 0 && recording == false) { 
			checkServerLine(line, bracket);
			recording = true;
		}	
		else if (recording == true) {
			chekLastchar(line);	
			block.push_back(line);
		}
		else if (line.find_first_not_of(" \t\n\r\f\v") != std::string::npos) 
			throw error_throw("unknown directive \"" + line + "\" - config file", false);
	//	std::cout << "line recorded = [" << line << "]" << std::endl;
		countBrackets(bracket, line);
		if (line[0] == '}' && bracket == 0 && recording == true) {
			servers.push_back(Server(block, servers, i));
			block.clear();
			recording = false;
			i++;
		}
	}
	ifs.close();
}
