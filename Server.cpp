/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 12:33:52 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/20 11:27:29 by bahommer         ###   ########.fr       */
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

Server::Server(std::vector<std::string> config, std::vector<Server> const& servers, int i)
	: _i(i), _socketfd(-1), _max_body_size(1024), _servers(servers), /*_ip(0), _port(0),
	 _server_name(0),*/ _socketIsSet(false) {

	memset(&_server_addr, 0, sizeof(_server_addr));
	memset(&_res, 0, sizeof(_res));

	void (Server::*ptr[PARAM])(std::string const&) =
		{ &Server::p_listen, &Server::p_host, &Server::p_server_name, &Server::p_bodySize };
	std::string keyword[] = {"listen", "host", "server_name", "client_max_body_size"};

	for (size_t i = 0; i < config.size() ; i++) {
		for (int j = 0; j < PARAM ; ++j) {
			if (config[i].find(keyword[j]) != std::string::npos) {
				(this->*ptr[j])(config[i]);
				break;
			}
		}
	}
	openSocket();	
	configServer();
}

Server::~Server(void) {}

void Server::openSocket(void) { // 1st check if open socket is necessary (config ip bind exist) 

	for (size_t i = 0; i < _servers.size(); i++) {
		if (_servers[i].getIp() == _ip && _servers[i].getPort() == _port) {
			_socketfd = _servers[i].getSocketfd();
			_socketIsSet = true;
			return;
		}
	}
	_socketfd = socket(AF_INET, SOCK_STREAM, 0); 
		if (_socketfd == -1) {
			std::cerr << "socket error:";
			throw std::runtime_error(strerror(errno));
		}	
	int yes = 1;	
	setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));	
}	

void Server::configServer(void) {
	
	struct addrinfo hints;
	struct addrinfo* current;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = _server_addr.sin_family; // = AF_INET = IPV4 for now AF_UNSPEC then?
	hints.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo(_ip.c_str(), _port.c_str(), &hints, &_res);
	if (ret != 0) {
		std::cerr << "getaddrinfo error: ";
		throw std::runtime_error(gai_strerror(ret));
	}
	if (_socketIsSet == false) { // Port + ip not already binded and listen
		for (current = _res; current != 0; current = current->ai_next) {
			if (bind(_socketfd, current->ai_addr, current->ai_addrlen) == 0) {
				break;
			} 
			if (current->ai_next == 0) {
				std::cerr << "bind error: ";
				throw std::runtime_error(strerror(errno));
			}	
		}
		ret = listen(_socketfd, MAX_CO);
		if (ret != 0) {
			std::cerr << "listen error: ";
			throw std::runtime_error(strerror(errno));
		}
	}	
	freeaddrinfo(_res);
}	

void Server::p_listen(std::string const& line) {

	size_t pos = line.find("listen");
	pos += std::string("listen").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}	
	_port = line.substr(pos, line.length() - 1 - pos);

	int int_port = atoi(_port.c_str());
	if (int_port < 1024 || int_port > 65535)
		throw std::runtime_error("Ports must be set between 1024 and 65535");

	uint16_t port = static_cast<uint16_t>(int_port);
	_server_addr.sin_port = htons(port); // converts port in network byte order 
	_server_addr.sin_family = AF_INET; // for IPV4

	std::cout << "port = " << _port << std::endl;
}	

void Server::p_host(std::string const& line) {
	
	size_t pos = line.find("host");
	pos+= std::string("host").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	_ip = line.substr(pos, line.length() - 1 - pos);
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //i should specify address?
	if (_ip == "localhost")
		_ip = "127.0.0.1";

	std::cout << "host = " << _ip << std::endl;
}

void Server::p_bodySize(std::string const& line) {

	size_t pos = line.find("client_max_body_size");
	pos+= std::string("client_max_body_size").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	_max_body_size = atoi(line.substr(pos).c_str());
	std::cout << "max body size = " << _max_body_size << std::endl;
}	

void Server::p_server_name(std::string const& line) {
	
	size_t pos = line.find("server_name");
	pos+= std::string("server_name").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}	
	_server_name = line.substr(pos, line.length() - 1 - pos);
	
	std::cout << "server_name " << _server_name << std::endl;
}	

std::string Server::getIp(void) const {
	return _ip;
}

std::string Server::getPort(void) const {
	return _port;
}	

int Server::getSocketfd(void) const {
	return _socketfd;
}	

sockaddr_in Server::getclientAddr(void) const {
	return _server_addr;
}	

std::string Server::getServerName(void) const {
	return _server_name;
}	

int	Server::getMaxBodySize(void) const {
	return _max_body_size;
}		
