#ifndef DELETE_HPP
#define DELETE_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"

class Delete: public Method {
	public:
	Delete(std::string path, std::string content);
	~Delete();
	void	execute_method();
};

#endif