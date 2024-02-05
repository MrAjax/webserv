/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mferracc <mferracc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 12:33:52 by bahommer          #+#    #+#             */
/*   Updated: 2024/02/05 09:14:26 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "../inc/parsing.hpp"
#include "../utils/utils.hpp"
#include "../inc/webserv.hpp"
#include "Location.hpp"

Server::Server(std::vector<std::string> config, std::vector<Server> const& servers, int i)
	: _i(i), _socketfd(-1), _max_body_size(-1), _servers(servers), _socketIsSet(false) 
{	

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
	configServer();
	setDefaultValue();
	server_log("Server " + int_to_str(_i) + " set up and ready to listen", INFO);
}

Server& Server::operator = (Server const& a) {
	if (this != &a) {

	_i = a._i;
	_socketfd = a._socketfd;
	_max_body_size = a._max_body_size;
	_error_pages = a._error_pages;
	_res = a._res;
	_ip = a._ip;
	_port = a._port;
	_server_name = a._server_name;
	_root = a._root;
	_location_error_page = a._location_error_page;
	_index = a._index;	
	_socketIsSet = a._socketIsSet;
	
	std::map<std::string, Location*>::const_iterator it;
		for (it = a._locations.begin(); it != a._locations.end(); ++it) {
			_locations[it->first] = new Location(*(it->second));
		}	
	}
	return *this;
}

Server::Server(Server const& a) {

	_i = a._i;
	_socketfd = a._socketfd;
	_max_body_size = a._max_body_size;
	_error_pages = a._error_pages;
	_res = a._res;
	_ip = a._ip;
	_port = a._port;
	_server_name = a._server_name;
	_root = a._root;
	_location_error_page = a._location_error_page;
	_index = a._index;	
	_socketIsSet = a._socketIsSet;
	
	std::map<std::string, Location*>::const_iterator it;
		for (it = a._locations.begin(); it != a._locations.end(); ++it) {
			_locations[it->first] = new Location(*(it->second));
		}	
}

Server::~Server(void) {

	std::map<std::string, Location*>::iterator it;
		for (it = _locations.begin(); it != _locations.end(); ++it) {
			delete it->second;
		}	
	}

void Server::openSocket(void) { 

	for (size_t i = 0; i < _servers.size(); i++) {
		if (_servers[i].getIp() == _ip && _servers[i].getPort() == _port) {
			_socketfd = _servers[i].getSocketfd();
			_socketIsSet = true;
			server_log("Socket of server " + int_to_str(_i) + " already set", INFO);
			return;
		}
	}
	_socketfd = socket(_res->ai_family, _res->ai_socktype, _res->ai_protocol); 
	if (_socketfd == -1) {
		throw error_throw("Socket error - parsing/Server.cpp", true);
	}
	int yes = 1;	
	if (setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		throw error_throw("setsockopt error during SO_REUSEADDR config - parsing/Server.cpp", true);
}	

void Server::configServer(void) {
	
	struct addrinfo hints;
	struct addrinfo* current;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // can be IPV4 or IPV6
	hints.ai_socktype = SOCK_STREAM;

	if (_ip.empty() == true || _port.empty() == true)
		throw error_throw("host and port must be specified - config file", false);

	int ret = getaddrinfo(_ip.c_str(), _port.c_str(), &hints, &_res);
	if (ret != 0) {
		server_log("getaddrinfo error - parsing/Server.cpp", ERROR);
		throw std::runtime_error(gai_strerror(ret));
	}
	openSocket();
	if (_socketIsSet == false) { // Port + ip not already binded and listen
		for (current = _res; current != 0; current = current->ai_next) {
			if (bind(_socketfd, current->ai_addr, current->ai_addrlen) == 0) {
				break;
			} 
			if (current->ai_next == 0) {
				throw error_throw("bind error - parsing/Server.cpp", true);
			}	
		}
		ret = listen(_socketfd, MAX_CO);
		if (ret != 0) {
			throw error_throw("listen error - parsing/Server.cpp", true);
		}
	}	
	freeaddrinfo(_res);
}	

void Server::setDefaultValue(void) {

	if (_max_body_size == -1)
		_max_body_size = 1024;
	if (_error_pages.empty() == true)
		_error_pages.push_back(404);
	if (_location_error_page.empty() == true)
		_location_error_page = "/errorpages";
	if (_index.empty() == true)
		_index.push_back("/index.html");
}		

void Server::p_listen(std::string const& line) {

	if (_port.empty() == false)
		throw error_throw ("On server " + int_to_str(_i) + " port setup twice", false);

	size_t pos = std::string("listen").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}	
	_port = line.substr(pos, line.length() - pos);

	int int_port = atoi(_port.c_str());
	if (int_port < 1024 || int_port > 65535)
		throw error_throw("Ports must be set between 1024 and 65535 - config file", false);

	server_log("Server " + int_to_str(_i) + " port is " + _port, INFO);
}

void Server::p_host(std::string const& line) {
	
	if (_ip.empty() == false)
		throw error_throw ("On server " + int_to_str(_i) + " host setup twice", false);

	size_t pos= std::string("host").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	_ip = line.substr(pos, line.length() - pos);
	if (_ip == "localhost")
		_ip = "127.0.0.1";

	server_log("Server " + int_to_str(_i) + " host is " + _ip, INFO);
}

void Server::p_bodySize(std::string const& line) {

	if (_max_body_size != -1) {
		throw error_throw ("On server " + int_to_str(_i) + " max_body_size setup twice", false);
	}	

	size_t pos = std::string("client_max_body_size").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	_max_body_size = atoi(line.substr(pos).c_str());
	if (_max_body_size < 1024 || _max_body_size > 15000) {
		throw error_throw("max_body_size must be set between 1024 and 15000 - config file", false);
	}	
	server_log("Server " + int_to_str(_i) + " max_body_size is " + int_to_str(_max_body_size), INFO);
}	

void Server::p_server_name(std::string const& line) {
	
	if (_server_name.empty() == false) {
		throw error_throw ("On server " + int_to_str(_i) + " server_name setup twice", false);
	}	
		
	size_t pos= std::string("server_name").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}	
	_server_name = line.substr(pos, line.length() - pos);
	server_log("Server " + int_to_str(_i) + " server_name is " + _server_name, INFO);
}	

void Server::p_root(std::string const& line) {

	if (_root .empty() == false) {
		throw error_throw ("On server " + int_to_str(_i) + " root setup twice", false);
	}	
		
	size_t pos = std::string("root").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	_root = line.substr(pos, line.length() - pos);
	server_log("Server " + int_to_str(_i) + " root is " + _root, INFO);
}

void Server::p_errorPage(std::string const& line) {
	
	if (_error_pages.empty() == false) {
		throw error_throw ("On server " + int_to_str(_i) + " error_pages setup twice", false);
	}	
		
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
		else { 
			size_t length = 0;	
			while (pos + length < line.size())
				length++;
			_location_error_page = line.substr(pos, length);
			break; //no text after location
		}
	}
}

void Server::p_index(std::string const& line) {

	if (_index.empty() == false) {
		throw error_throw ("On server " + int_to_str(_i) + " index setup twice", false);
	}	
	size_t pos = std::string("index").length();

	while (pos < line.length()) {
		while (pos < line.length() && std::isspace(line[pos])) {
			++pos;
		}
		size_t end = line.find_first_of(" \t\n\r\f\v", pos);
		if (end == std::string::npos)
			_index.push_back(line.substr(pos, line.length() - pos));
		else
			_index.push_back(line.substr(pos, end - pos));
		pos = end;	
	}
}	
	
std::string Server::settempLocation(std::string line) {

	size_t start = std::string("location").length();
	while (start < line.length() && std::isspace(line[start])) {
			++start;
	}
	size_t end = line.find_first_of(" {\t\n\r\f\v", start);
	if (end == std::string::npos) 
		throw error_throw(line + " unopened bracket", false);
	if (end == start)
		throw error_throw(line + " must be specific", false);
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

std::vector<std::string> Server::getIndex(void) const {
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
