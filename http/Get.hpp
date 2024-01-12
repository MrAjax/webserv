#ifndef GET_HPP
# define GET_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"

class Get: public Method {
	public:
	void	execute_method();
};

#endif