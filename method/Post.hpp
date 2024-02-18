#ifndef POST_HPP
# define POST_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"

class Post: public Method {
	std::string	_guess_mime_type(std::string &body);
	void		_set_post_path(std::string path);
	void		_fill_post_file(Server &serv, std::string body);
public:
	Post(std::string path, std::string raw_path, std::string root, std::string content, std::string body_request, std::string connection_status);
	~Post();
	void	execute_method(Server &serv);
};

#endif