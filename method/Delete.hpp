#ifndef DELETE_HPP
#define DELETE_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"

class Delete: public Method {
	public:
	Delete(const std::string &path, const std::string &content, const std::string &connection_status);
	~Delete();
	void	remove_file(Server &serv);
	void	execute_method(Server &serv);
};

#endif