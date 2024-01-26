#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "../inc/webserv.hpp"
#include "../HttpRequest.hpp"
#include "../method/Method.hpp"
#include "../method/Get.hpp"
#include "../method/Post.hpp"
#include "../method/Delete.hpp"

class HttpResponse {
	std::string		_method;
	int				_method_code;
	std::string		_path;
	std::string		_server_name;
/*		For post and delete methods		*/
	std::string		_body_request;
/*		Not set in the consructor		*/
	int				_status_code;
	std::string		_status_msg;
	std::string		_contentType;
	std::string		_header;
	std::string		_body;
	std::string		_response;
	Method	*_execute_method(int method_code, Server &serv);
public:
	HttpResponse(HttpRequest &req, Server &serv);

	~HttpResponse();

	std::string	get_response(Server &serv);
	std::string	get_header();
};

#endif