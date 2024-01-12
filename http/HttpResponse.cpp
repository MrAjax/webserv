#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest &req) {
	// On presuppose au depart que le header est valide
	_protocol = req.getProtocol();
	_header = req.getHeader();
	_response = _protocol + " " + _status_code + _status_msg + "\n" + _header;

	std::cout << "\n*********************************************\n*********************************************\n" << _response << "\n*********************************************\n";
	if (getResponse())
		throw std::runtime_error("ERROR: *****\n");
}

HttpResponse::~HttpResponse() {}

int	HttpResponse::getResponse() {
	
	std::string	resp;


	_response = resp;
	return (1);
}

void HttpResponse::sendhtml(int connfd, std::string ContentType, std::string input)
{
	std::ifstream file(input.c_str());

	if (file.is_open())
	{
		std::stringstream ss;
		ss << file.rdbuf();
		file.close();
	
	std::string htmlcontent = ss.str();

	std::string response;
	response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: " + ContentType + "\r\n";
	std::stringstream sss;
	sss << htmlcontent.length();
	response += "Content-Length: " + sss.str() + "\r\n\r\n";
    response += htmlcontent;

	write(connfd, response.c_str(), response.length());
	}
	else{
		std::cerr << "Error HTML" << std::endl;
	}

}