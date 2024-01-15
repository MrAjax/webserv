#ifndef METHOD_HPP
# define METHOD_HPP

#include "../inc/webserv.hpp"
#include "HttpStatusCode.hpp"

enum {GET, POST, DELETE};

class Method {
	std::string	_path;
	std::string	_header;
	std::string	_body;
	int			_status_code;
public:
	//Method();
	Method(std::string path);
	virtual ~Method();

	virtual void	execute_method() = 0;
	
	std::string		get_path();
	std::string		get_header();
	std::string		get_body();
	int				get_status_code();

	void			set_header(std::string header);
	void			set_body(std::string body);
	void			set_statuscode(int code);
};

#endif