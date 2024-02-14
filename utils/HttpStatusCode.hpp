#ifndef HTTPSTATUSCODE_HPP
# define HTTPSTATUSCODE_HPP

#include "../inc/webserv.hpp"

class HttpStatusCode {
	void	init_class();
	static	std::map<int, std::string>	_http_status_code;
	static	std::string					_http_error_list;
	static	bool						_initialized;
	HttpStatusCode();
	~HttpStatusCode();
	std::map<int, std::string>	&_get_http_status_code();
public:
	static std::string			get_error_msg(int code);
};

#endif