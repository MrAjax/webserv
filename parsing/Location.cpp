#include "Location.hpp"

Location::Location(std::vector<std::string> config) 
{
	void (Location::*ptr[PARAM_LOC])(std::string const&) =
		{&Location::p_allow_methods, &Location::p_root };
	std::string keyword[] = {"allow_methods", "root"};

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
/*	for(size_t i = 0; i < _allow_methods.size(); ++i) {
		std::cout << "[" << _allow_methods[i] << "]}";
	}	
	std::cout << "END" << std::endl;*/
}	

std::vector<std::string> Location::getallow_methods(void) const {
	return _allow_methods;
}

std::string Location::getRoot(void) const {
	return _root;
}
