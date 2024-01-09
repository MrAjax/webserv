#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "http.hpp"
#include "HttpRequest.hpp"

class HttpResponse {
	std::string	_version;
	std::string	_status_code;
	std::string	_status_msg;
	std::string	_header;
public:
	HttpResponse();
	HttpResponse();
};

#endif