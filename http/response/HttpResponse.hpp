#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "../inc/webserv.hpp"
#include "../request/HttpRequest.hpp"
#include "../method/Method.hpp"
#include "../method/Get.hpp"
#include "../method/Post.hpp"
#include "../method/Delete.hpp"
#include "../utils/HttpStatusCode.hpp"
#include "ContentTypeFinder.hpp"

class HttpResponse {
	std::string	_contentType;
	std::string	_method;
	int			_method_code;
	std::string	_path;
	std::string	_body_request;
	int			_status_code;
	std::string	_status_msg;
	std::string	_header;
	std::string	_response;
public:
	HttpResponse(HttpRequest &req);

	~HttpResponse();

	std::string	get_response();
	std::string	get_header();
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