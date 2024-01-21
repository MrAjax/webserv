#ifndef HTTPERROR_HPP
#define HTTPERROR_HPP

#include "../inc/webserv.hpp"

class HttpError {
	std::string		_path;
	int				_status_code;
	std::string		_status_msg;
	std::string		_contentType;
	std::string		_header;
	std::string		_body;
	std::string		_response;
public:
	HttpError(int err);
	~HttpError();

	std::string		get_response();
};

#endif