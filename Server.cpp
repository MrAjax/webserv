/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 12:33:52 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/15 14:57:00 by bahommer         ###   ########.fr       */
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
 			struct sockaddr {
          		 sa_family_t     sa_family;      Address family 
           		char            sa_data[];       Socket address 
    	  	 };
 			struct sockaddr_in {
        	   sa_family_t     sin_family;      AF_INET 
          		 in_port_t       sin_port;        Port number 
          		struct in_addr  sin_addr;        IPv4 address 
       		};
	   		 struct in_addr {
           		in_addr_t s_addr;
       		};
	1st config sockaddr_in then cast in sockaddr
*/

#include "Server.hpp"
#include "parsing.hpp"
#include "serv_config.hpp"

Server::Server(std::vector<std::string> config, serv_config const& webserv) : _webserv(webserv) 
{
	memset(&_server_addr, 0, sizeof(_server_addr));

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
	hints.ai_family = _server_addr.sin_family; // = AF_INET = IPV4 for now
	hints.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo(_node.c_str(), _service.c_str(), &hints, &_res);
	if (ret != 0) {
		std::cerr << "getaddrinfo error: " << gai_strerror(ret) << std::endl;
	}
	std::cout << "Fd = " << _webserv.getSocketfd() << std::endl;
//	int ret = bind(
			
}	

void Server::p_listen(std::string const& line) {

	size_t pos = line.find("listen");
	pos += 6;
	_service = line.substr(pos, line.length() - 1 - pos);

	int int_port = atoi(_service.c_str());
	/*define PORT_MAX 65535 PORT_MIN 0*/

	uint16_t port = static_cast<uint16_t>(int_port);

	//std::cout << "port = " << port << std::endl;
	_server_addr.sin_port = htons(port); // converts port in network byte order 
	_server_addr.sin_family = AF_INET; // for IPV4

	std::cout << "port = " << _service << std::endl;
}	

void Server::p_host(std::string const& line) {
	
	size_t pos = line.find("host");
	pos+= 5;
	_node = line.substr(pos, line.length() - 1 - pos);
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //i should specify address?

	std::cout << "host = " << _node << std::endl;
}

void Server::p_server_name(std::string const& line) {

	std::cout << "server_name " << line << std::endl;
}	
