#include "Method.hpp"

//Method::Method() {};

Method::Method(std::string path, std::string content): _path(path), _content_type(content), _header("HTTP/1.1"), _body(""), _status_code(0) {}

Method::~Method() {}

void	Method::execute_method() {}

std::string	Method::get_path() {
	return _path;
}

std::string	Method::get_content_type() {
	return _content_type;
}

std::string	Method::get_header() {
	return _header;
}

std::string	Method::get_body() {
	return _body;
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