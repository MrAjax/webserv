#ifndef CONTENTTYPEFINDER_HPP
# define CONTENTTYPEFINDER_HPP

#include "../inc/webserv.hpp"
#include "utils.hpp"

class ContentTypeFinder {
	std::map<std::string, std::string>	_type_dictionnary;
	ContentTypeFinder();
	~ContentTypeFinder();
	std::map<std::string, std::string>	_get_map();
public:	
	static	std::string	get_content_type(std::string path);
};

#endif