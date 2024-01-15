#ifndef GET_HPP
# define GET_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"
#include "utils.hpp"

class Get: public Method {
public:
	Get(std::string path);
	~Get();
	
	void	get_text();
	void	execute_method();
};

#endif