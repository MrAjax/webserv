#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "../inc/webserv.hpp"
#include "HttpRequest.hpp"
#include "Method.hpp"
#include "Get.hpp"
#include "Post.hpp"
#include "Delete.hpp"

class HttpResponse {
	std::string	_method;
	std::string	_path;
	std::string	_protocol;
	std::string	_status_code;
	std::string	_status_msg;
	std::string	_header;
	std::string	_response;
public:
	HttpResponse(HttpRequest &req);

	~HttpResponse();

	void sendhtml(int connfd, std::string ContentType, std::string input);
	std::string	get_response();
};

// Building and storing the HTTP response (status line, header, body...)
// Setting the appropriate status code

/*

	1- Locates the appropriate document and returns it.

*/

/*

	=== HTTP RESPONSE FORM ===

	HTTP/[VER] [CODE] [TEXT]
	Field1: Value1
	Field2: Value2

	...Document content here...

*/

#endif