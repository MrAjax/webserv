/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 12:33:52 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/26 15:59:06 by bahommer         ###   ########.fr       */
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
#include "../inc/parsing.hpp"
#include "../utils/utils.hpp"
#include "../inc/webserv.hpp"
#include "Location.hpp"

Server::Server(std::vector<std::string> config, std::vector<Server> const& servers, int i)
	: _i(i), _socketfd(-1), _max_body_size(1024), _error_pages(1, 404), _servers(servers), _ip(""), _port(""), _server_name(""), _root(""), _location_error_page("/default"), _socketIsSet(false) {

	memset(&_server_addr, 0, sizeof(_server_addr));
	memset(&_res, 0, sizeof(_res));

	void (Server::*ptr[PARAM])(std::string const&) =
		{ &Server::p_listen, &Server::p_host, &Server::p_server_name, &Server::p_bodySize,
			&Server::p_root, &Server::p_errorPage, &Server::p_index };
	std::string keyword[] = {"listen", "host", "server_name", "client_max_body_size", "root",
		"error_page", "index"};

	std::vector<std::string> block;
	std::string tempLocation;
	bool recording = false;

	for (size_t i = 0; i < config.size() ; i++) {
	//	std::cout << "line recorded = [" << config[i] << "]" << std::endl;
		int j = 0;
		
		if (recording == true && config[i][0] == '}') {
			Location* tempPtr = new Location(block);
			_locations.insert(std::make_pair(tempLocation, tempPtr));
			block.clear();
			recording = false;
		}	
		else if (config[i].compare(0, 8, "location") == 0) {
			tempLocation = settempLocation(config[i]);
			recording = true;
		}	
		else if (recording == true)
			block.push_back(config[i]);
		else {	
			while (j < PARAM) {
				if (config[i].compare(0, keyword[j].length(), keyword[j]) == 0) {
					(this->*ptr[j])(config[i]);
					break;
				}
				j++;	
			}	
		}	
		if (j == PARAM && config[i][0] != '}') // no condition
			throw error_throw("unknown directive \"" + config[i] + "\" - config file", false);
	}
	server_log("Server " + int_to_str(_i + 1) + " parsed", INFO);
	openSocket();	
	configServer();
	server_log("Server " + int_to_str(_i + 1) + " set up and ready to listen", INFO);
}

Server::~Server(void) {
	
//	std::map<std::string, Location*>::iterator it;
//	for (it = _locations.begin(); it != _locations.end(); ++it) {
//		delete it->second;
//	}	
}

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
			error_throw("Socket error - parsing/Server.cpp", true);
		}
	int yes = 1;	
	if (setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		error_throw("setsockopt error - parsing/Server.cpp", true);
}	

void Server::configServer(void) {
	
	struct addrinfo hints;
	struct addrinfo* current;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = _server_addr.sin_family; // = AF_INET = IPV4 for now AF_UNSPEC then?
	hints.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo(_ip.c_str(), _port.c_str(), &hints, &_res);
	if (ret != 0) {
		server_log("getaddrinfo error - parsing/Server.cpp", ERROR);
		throw std::runtime_error(gai_strerror(ret));
	}
	if (_socketIsSet == false) { // Port + ip not already binded and listen
		for (current = _res; current != 0; current = current->ai_next) {
			if (bind(_socketfd, current->ai_addr, current->ai_addrlen) == 0) {
				break;
			} 
			if (current->ai_next == 0) {
				error_throw("bind error - parsing/Server.cpp", true);
			}	
		}
		ret = listen(_socketfd, MAX_CO);
		if (ret != 0) {
			error_throw("listen error - parsing/Server.cpp", true);
		}
	}	
	freeaddrinfo(_res);
}	

void Server::p_listen(std::string const& line) {

	size_t pos = std::string("listen").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}	
	_port = line.substr(pos, line.length() - pos);

	int int_port = atoi(_port.c_str());
	if (int_port < 1024 || int_port > 65535)
		error_throw("Ports must be set between 1024 and 65535 - config file", false);

	uint16_t port = static_cast<uint16_t>(int_port);
	_server_addr.sin_port = htons(port); // converts port in network byte order 
	_server_addr.sin_family = AF_INET; // for IPV4

	std::cout << "port = " << _port << std::endl;
}	

void Server::p_host(std::string const& line) {
	
	size_t pos= std::string("host").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	_ip = line.substr(pos, line.length() - pos);
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //i should specify address?
	if (_ip == "localhost")
		_ip = "127.0.0.1";

	std::cout << "host = " << _ip << std::endl;
}

void Server::p_bodySize(std::string const& line) {

	size_t pos = std::string("client_max_body_size").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	_max_body_size = atoi(line.substr(pos).c_str());
	std::cout << "max body size = " << _max_body_size << std::endl;
}	

void Server::p_server_name(std::string const& line) {
	
	size_t pos= std::string("server_name").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}	
	_server_name = line.substr(pos, line.length() - pos);
	
	std::cout << "server_name " << _server_name << std::endl;
}	

void Server::p_root(std::string const& line) {

	size_t pos = std::string("root").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	_root = line.substr(pos, line.length() - pos);
	std::cout << "root = " << _root << std::endl;
}

void Server::p_errorPage(std::string const& line) {
	
	_error_pages.clear(); //clean defaut error page;
	size_t pos = std::string("error_page").length();
	while (pos < line.length()) {
		while (pos < line.length() && std::isspace(line[pos])) {
			++pos;
		}	
		if (isdigit(line[pos])) {
			size_t length = 0;
			while (isdigit(line[pos + length])) 
				length++;
			std::string temp = line.substr(pos, length);
			int number = atoi(temp.c_str());
			if (number < 300 || number > 599)
				throw error_throw("Value \"" + temp + "\" must be between 300 and 500", false);
			else
				_error_pages.push_back(number);
			pos += length;
		}
		else if (line[pos] == ';') 
			throw error_throw("No error page location - config file", false);
		else { 
			size_t length = 0;	
			while (pos + length < line.size() && line[pos + length] != ';')
				length++;
			_location_error_page = line.substr(pos, length);
			break; //no text after location
		}
	}
	std::cout << "Error pages: ";
	for (size_t i = 0; i < _error_pages.size(); i++) {
		std::cout << _error_pages[i] << " ";
	}
	std:: cout << "Location: " << _location_error_page << std::endl;
}

void Server::p_index(std::string const& line) {

	size_t pos = std::string("index").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	_index = line.substr(pos, line.length() - pos);
	std::cout << "index = " << _index << std::endl;
}	
	
std::string Server::settempLocation(std::string line) {

	size_t start = std::string("location").length();
	while (start < line.length() && std::isspace(line[start])) {
			++start;
	}
	size_t end = line.find_first_of(" {\t\n\r\f\v", start);
	if (end == std::string::npos) 
		error_throw(line + " unopened bracket", false);
	return (line.substr(start, end - start));	
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

std::string Server::getRoot(void) const {
	return _root;
}	

std::vector<int> Server::getErrorPages(void) const {
	return _error_pages;
}

std::string Server::getLocationErrorPage(void) const {
	return _location_error_page;
}	

std::string Server::getIndex(void) const {
	return _index;
}	

Location* Server::getLocation(std::string type) const {

	std::map<std::string, Location*>::const_iterator it;

	it = _locations.find(type);
	if (it != _locations.end()) 
		return it->second;
	else {
		server_log("This location " + type + " does not exist", ERROR);
		return NULL;
	}
}
