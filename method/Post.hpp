#ifndef POST_HPP
# define POST_HPP

#include "../inc/webserv.hpp"
#include "Method.hpp"

class Post: public Method {
	std::string	_guess_mime_type(std::string &body);
	void		_set_post_path(std::string path);
	void		_set_post_path_Test();
	void		_fill_post_file(Server &serv, std::string body);
public:
	Post(const std::string &path, const std::string &raw_path, const std::string &root, const std::string &content, const std::string &connection_status, std::string &body_request);
	~Post();
	void	execute_method(Server &serv);
};

#endif