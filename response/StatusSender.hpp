#ifndef STATUSSENDER_HPP
#define STATUSSENDER_HPP

#include "../inc/webserv.hpp"
#include "../utils/utils.hpp"
#include "../parsing/Server.hpp"
#include "../method/Method.hpp"

class StatusSender {
	std::string		_path;
	int				_status_code;
	std::string		_status_msg;
	std::string		_contentType;
	std::string		_header;
	std::string		_body;
	std::string		_response;
	std::string		_server_path;
	std::string		_error_pages;

	StatusSender(int err, Server &serv);
	~StatusSender();

	std::string		_error_400();
	std::string		_error_401();
	std::string		_error_403();
	std::string		_error_404();
	std::string		_error_500();
	std::string		_error_generic(int err);
	std::string		_get_response();
public:
	static	std::string		send_status(int err, Server &serv);
};

#endif