#ifndef HTTPSTATUSCODE_HPP
# define HTTPSTATUSCODE_HPP

#include "../inc/webserv.hpp"

class HttpStatusCode {
	std::map<int, std::string>	_http_status_code;
	std::string					_http_error_list;
public:
	HttpStatusCode();
	~HttpStatusCode();

	std::map<int, std::string>	&get_http_status_code();
	static std::string			get_error_msg(int code);
};

/* class HttpStatusCode {
public:
	HttpStatusCode();
	~HttpStatusCode();

//	std::map<int, std::string>	&get_http_status_code();
	static std::string			get_error_msg(int key);
}; */

#endif