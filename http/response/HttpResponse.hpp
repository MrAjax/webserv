#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "../inc/webserv.hpp"
#include "../HttpRequest.hpp"
#include "../method/Method.hpp"
#include "../method/Get.hpp"
#include "../method/Post.hpp"
#include "../method/Delete.hpp"
#include "../utils/HttpStatusCode.hpp"

class HttpResponse {
// Request Elements
	int			_connfd;
	std::string	_contentType;
	std::string	_input;
	
	std::string	_method;
	int			_method_code;
	std::string	_path;
// Response Elements
	std::string	_protocol;
	int			_status_code;
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