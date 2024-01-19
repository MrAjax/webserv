#ifndef CONTENTTYPEFINDER_HPP
# define CONTENTTYPEFINDER_HPP

#include "../inc/webserv.hpp"

class ContentTypeFinder {
	std::map<std::string, std::string>	_type_dictionnary;
public:
	ContentTypeFinder();
	~ContentTypeFinder();

	std::map<std::string, std::string>	get_map();
	
	static	std::string	get_content_type(std::string path);
};

#endif