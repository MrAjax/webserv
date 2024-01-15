/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_config.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 11:13:25 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/15 15:49:51 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serv_config.hpp"
#include "Server.hpp"

serv_config::serv_config(char* file) {

	_ifs.open(file);

	if (!_ifs)
	/*add exception */
		return ;
	config_webserv();	
	parse_blocks();
}	

serv_config::~serv_config(void) {}

void serv_config::config_webserv(void) {

	socketfd = socket(AF_INET, SOCK_STREAM, 0); 
	/* check error */
}	

void serv_config::parse_blocks(void) { // Cut every server block ("server {" to ... "}")
									// in std::vector<std::string> and create a server class member
	std::string line;
	std::vector<std::string> block;
	bool recording = false;
	int	i = 0;

	while (getline(_ifs, line)) {
		if (line.find("server {") != std::string::npos) {
			recording = true;
		}
		else if (line[0] == '}' && recording == true) {
			_servers.push_back(Server(block, *this, i));
			block.clear();
			recording = false;
			i++;
		}
		else if (recording == true) {
			block.push_back(line);
		}
	}
	_ifs.close();
}

int	serv_config::getSocketfd( void ) const {
	return socketfd;
}	
