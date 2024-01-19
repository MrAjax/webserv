#ifndef POST_HPP
# define POST_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"

class Post: public Method {
	public:
	Post(std::string path, std::string content);
	~Post();
	void	execute_method();
};

#endif