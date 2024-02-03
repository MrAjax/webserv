#ifndef CGI_HPP
# define CGI_HPP

#include "../inc/webserv.hpp"
#include "../utils/utils.hpp"

class Cgi {
	Cgi();
	~Cgi();
public:
	static	int	exec_cgi(std::string &path, std::string &output, std::string &input);
};

#endif