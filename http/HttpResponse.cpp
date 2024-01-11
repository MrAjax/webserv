#include "http.hpp"

HttpResponse::HttpResponse(HttpRequest &req) {
	if (req.parsingHeader())
		throw std::runtime_error("ERROR: Invalid Header\n");
	_protocol = req.getProtocol();
	_header = req.getHeader();
	_response = _protocol + " " + _status_code + _status_msg + "\n" + _header;

	if (getResponse())
		throw std::runtime_error("ERROR: *****\n");
}

//HttpResponse::HttpResponse(const HttpResponse &rep) {}

//HttpResponse	&HttpResponse::operator=(const HttpResponse &rep) {}

HttpResponse::~HttpResponse() {}

int	HttpResponse::getResponse() {
	
	std::string	resp;


	_response = resp;
	return (1);
}