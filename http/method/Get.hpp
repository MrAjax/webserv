#ifndef GET_HPP
# define GET_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"
#include "../utils/utils.hpp"

class Get: public Method {
public:
	Get(std::string path, std::string content);
	~Get();
	
	void	get_text();
	void	execute_method();
};

#endif