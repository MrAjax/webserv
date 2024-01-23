#ifndef STATUSSENDER_HPP
#define STATUSSENDER_HPP

#include "../inc/webserv.hpp"
#include "../utils/utils.hpp"

class StatusSender {
	std::string		_path;
	int				_status_code;
	std::string		_status_msg;
	std::string		_contentType;
	std::string		_header;
	std::string		_body;
	std::string		_response;

	StatusSender(int err);
	~StatusSender();

	std::string		_error_400();
	std::string		_error_401();
	std::string		_error_403();
	std::string		_error_404();
	std::string		_error_500();
	std::string		_get_response();
public:
	static	std::string		send_status(int err);
};

#endif