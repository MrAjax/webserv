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
/*		For post and delete methods		*/
	std::string		_body_request;
/*		Not set in the consructor		*/
	int				_status_code;
	std::string		_status_msg;
	std::string		_contentType;
	std::string		_header;
	std::string		_body;
	std::string		_response;
	Method	*_execute_method(int method_code);
public:
	HttpResponse(HttpRequest &req);

	~HttpResponse();

	std::string	get_response();
	std::string	get_header();
};

#endif