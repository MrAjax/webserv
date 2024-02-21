#ifndef GET_HPP
# define GET_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"

class Get: public Method {
public:
	Get(const std::string &path, const std::string &content, const std::string &connection_status);
	~Get();
	
	void	execute_method(Server &serv);
};

#endif