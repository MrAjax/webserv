#ifndef DELETE_HPP
#define DELETE_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"

class Delete: public Method {
	public:
	void	execute_method();
};

#endif