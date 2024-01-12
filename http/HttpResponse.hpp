#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "../inc/webserv.hpp"
#include "HttpRequest.hpp"

class HttpResponse {
	std::string					_protocol;
	std::string					_status_code;
	std::string					_status_msg;
	std::string					_header;
	std::string					_response;
public:
	HttpResponse(HttpRequest &req);

	~HttpResponse();

	int	getResponse();

	void sendhtml(int connfd, std::string ContentType, std::string input);
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