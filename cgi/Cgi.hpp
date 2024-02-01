#ifndef CGI_HPP
# define CGI_HPP

#include "../inc/webserv.hpp"

class Cgi {
	Cgi();
	~Cgi();
	std::ifstream	_cgi_file;
public:
	static	int	exec_cgi(std::string path);
};

#endif