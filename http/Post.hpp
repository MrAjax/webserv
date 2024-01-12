#ifndef POST_HPP
# define POST_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"

class Post: public Method {
	public:
	void	execute_method();
};

#endif