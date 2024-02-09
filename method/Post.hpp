#ifndef POST_HPP
# define POST_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"

class Post: public Method {
	public:
	Post(std::string path, std::string content, std::string body_request, std::string connection_status);
	~Post();
	void	fill_post_file(Server &serv, std::string query_string);
	void	execute_method(Server &serv);
};

#endif