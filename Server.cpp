/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 12:33:52 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/11 16:07:14 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
       The addrinfo structure used by getaddrinfo() contains the following fields:

           struct addrinfo {
               int              ai_flags;
               int              ai_family;
               int              ai_socktype;
               int              ai_protocol;
               socklen_t        ai_addrlen;
               struct sockaddr *ai_addr;
               char            *ai_canonname;
               struct addrinfo *ai_next;
           };
*/


#include "Server.hpp"
#include "parsing.hpp"

Server::Server(std::vector<std::string> config) 
{
	void (Server::*ptr[PARAM])(std::string const&) =
		{ &Server::p_listen, &Server::p_host, &Server::p_server_name };
	std::string keyword[] = {"listen", "host", "server_name"};

	for (size_t i = 0; i < config.size() ; i++) {
		for (int j = 0; j < PARAM ; ++j) {
			if (config[i].find(keyword[j]) != std::string::npos) {
				(this->*ptr[j])(config[i]);
				break;
			}
		}
	}	
	configServer();
}

Server::~Server(void) {}

void Server::configServer(void) {
	
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; //socket accept connexion
	hints.ai_protocol = 0;

/*getaddrinfo node = NULL : server listen all network, */
	int ret = getaddrinfo(NULL, _service, &hints, &_res);
	std::cout << "ret = " << ret << std::endl;

}	

void Server::p_listen(std::string const& line) {

	size_t pos = line.find("listen");
	pos+= 6;
	std::string temp = line.substr(pos);
	int port = atoi(temp.c_str());
	/*define PORT_MAX PORT_MIN*/
	std::cout << "port = " << port << std::endl;
	_service = temp.c_str();
}	

void Server::p_host(std::string const& line) {
	
	std::cout << "host " << line << std::endl;
}

void Server::p_server_name(std::string const& line) {

	std::cout << "server_name " << line << std::endl;
}	
