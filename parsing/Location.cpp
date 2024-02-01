#include "Location.hpp"
#include "../utils/utils.hpp"

Location::Location(std::vector<std::string> config) : _autoindex(false) 
{
	void (Location::*ptr[PARAM_LOC])(std::string const&) =
		{&Location::p_allow_methods, &Location::p_httpRedirection, &Location::p_root, &Location::p_autoindex,
			&Location::p_index	};
	std::string keyword[] = {"allow_methods", "return", "root", "autoindex", "index"};

	for (size_t i = 0; i < config.size() ; i++) {
	//	std::cout << "line recorded = [" << config[i] << "]" << std::endl;
		int j = 0;
		while (j < PARAM_LOC) {
			if (config[i].compare(0, keyword[j].length(), keyword[j]) == 0) {
				(this->*ptr[j])(config[i]);
				break;
			}
			j++;	
		}	
	//	if (j == PARAM && config[i][0] != '}') // no condition
	//		throw error_throw("unknown directive \"" + config[i] + "\" - config file", false);
	}
}

Location::Location( Location const& a) {

	_root = a._root;
	_return = a._return;
	_index = a._index;
	_allow_methods = a._allow_methods;
	_autoindex = a._autoindex;
}

Location& Location::operator= (Location const& a) {

	if (this != &a) {
		_root = a._root;
		_return = a._return;
		_index = a._index;
		_allow_methods = a._allow_methods;
		_autoindex = a._autoindex;
	}
	return *this;
}	
		
Location::~Location(void) {}

void Location::p_root(std::string const& line) {

	size_t pos = std::string("root").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	_root = line.substr(pos, line.length() - pos);
	std::cout << "root = " << _root << std::endl;
}

void Location::p_allow_methods(std::string const& line) {

	size_t pos = std::string("allow_methods").length();
	while (pos < line.length()) {
		while (pos < line.length() && std::isspace(line[pos])) {
			++pos;
		}
		size_t end = line.find_first_of(" \t\n\r\f\v", pos);
		if (end == std::string::npos)
			_allow_methods.push_back(line.substr(pos, line.length() - pos));
		else
			_allow_methods.push_back(line.substr(pos, end - pos));
		pos = end;	
	}

		for(size_t i = 0; i < _allow_methods.size(); ++i) {
		std::cout << "[" << _allow_methods[i] << "]}";
	}	
	std::cout << "END" << std::endl;
}	

void Location::p_httpRedirection(std::string const& line) {

	size_t pos = std::string("return").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	_return = line.substr(pos, line.length() - pos);
	std::cout << "http redirection = " << _return << std::endl;
}	

void Location::p_autoindex(std::string const& line ) {

	size_t pos = std::string("autoindex").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	std::string temp = line.substr(pos, line.length() - pos);
	std::cout << "temp [" << temp << "]" << std::endl;
	if (temp == "on")
		_autoindex = true;
	else if (temp == "off")
		_autoindex = false;
	else
		throw error_throw("invalid value " + temp + "in autoindex directive - config file", false);
	std::cout << "autoindex = " << _autoindex << std::endl;
}	

void Location::p_index(std::string const& line) {

	size_t pos = std::string("index").length();
	while (pos < line.length() && std::isspace(line[pos])) {
		++pos;
	}
	_index = line.substr(pos, line.length() - pos);
	std::cout << "index = " << _index << std::endl;
}	

std::vector<std::string> Location::getallow_methods(void) const {
	return _allow_methods;
}

std::string Location::getRoot(void) const {
	return _root;
}

std::string Location::getReturn(void) const {
	return _return;
}	

bool	Location::getAutoindex(void) const {
	return _autoindex;
}	

std::string Location::getIndex(void) const {
	return _index;
}	
