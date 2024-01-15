#ifndef DELETE_HPP
#define DELETE_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"

class Delete: public Method {
	public:
	Delete(std::string path);
	~Delete();
	void	execute_method();
};

#endif