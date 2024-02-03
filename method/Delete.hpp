#ifndef DELETE_HPP
#define DELETE_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"

class Delete: public Method {
	public:
	Delete(std::string path, std::string content);
	~Delete();
	void	remove_file(Server &serv);
	void	remove_line(std::string arg, Server &serv);
	void	execute_method(Server &serv);
};

#endif