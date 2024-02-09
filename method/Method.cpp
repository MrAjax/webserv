#include "Method.hpp"

//Method::Method() {};

Method::Method(std::string path, std::string content, std::string connection_status): _path(path), _content_type(content), _connection_status(connection_status), _header(""), _body(""), _status_code(0) {
	if (_connection_status.empty())
		_connection_status = "keep-alive";
}

Method::Method(std::string path, std::string content, std::string connection_status, std::string body_request): _path(path), _content_type(content), _connection_status(connection_status), _body_request(body_request), _header("HTTP/1.1"), _body(""), _status_code(0) {
	if (_connection_status.empty())
		_connection_status = "keep-alive";
}

Method::~Method() {}

void	Method::execute_method(Server &serv) {(void)serv;}

std::string	Method::get_path() {
	return _path;
}

std::string	Method::get_content_type() {
	return _content_type;
}

std::string	Method::get_connection_status() {
	return _connection_status;
}

std::string	Method::get_header() {
	return _header;
}

std::string	Method::get_body() {
	return _body;
}

std::string	Method::get_body_request() {
	return _body_request;
}

int	Method::get_status_code () {
	return _status_code;
}

void Method::set_header(std::string header) {
	_header += header;
}

void Method::set_body(std::string body) {
	_body = body;
}

void	Method::set_statuscode(int code) {
	_status_code = code;
}