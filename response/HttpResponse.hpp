#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "../inc/webserv.hpp"
#include "../request/HttpRequest.hpp"
#include "../method/Method.hpp"
#include "../method/Get.hpp"
#include "../method/Post.hpp"
#include "../method/Delete.hpp"

class HttpResponse {
	std::string		_method;
	int				_method_code;
	std::string		_path;
	std::string		_server_path;
	std::vector<std::string> _index_list;
/*		For post and delete methods		*/
	std::string		_body_request;
/*		Not set in the consructor		*/
	int				_status_code;
	bool			_is_cgi;
	std::string		_status_msg;
	std::string		_contentType;
	std::string		_header;
	std::string		_body;
	std::string		_response;
	
	std::string	_find_index_path(Server &serv);
	Method		*_execute_method(int method_code, Server &serv);
public:
	HttpResponse(HttpRequest &req, Server &serv);

	~HttpResponse();

	std::string	get_response(Server &serv);
	std::string	get_header();
};

#endif
