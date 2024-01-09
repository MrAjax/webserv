#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "http.hpp"

class HttpResponse {
	std::string	_version;
	std::string	_status_code;
	std::string	_status_msg;
	std::string	_header;
public:
	HttpResponse(HttpRequest &req);
	HttpResponse(const HttpResponse &rep);
	HttpResponse	&operator=(const HttpResponse &rep);
	~HttpResponse();
};

// Building an storing the HTTP response (status line, header, body...)
// Setting the appropriate status code

#endif