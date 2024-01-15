#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest &req) {
	// Recuperer ces infos de la classe Request
	(void)req;
	HttpStatusCode	codeHttp;

	_method = "GET";
	_path = "/";
//	_protocol = "HTTP/1.1";
//	_header = "Host";
//	_status_code = 200;
//	_status_msg = codeHttp.get_http_status_code().find(200)->second;
//	_response = _protocol + " " + _status_code + _status_msg + "\r\n" + _header;
}

HttpResponse::~HttpResponse() {}

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

std::string	HttpResponse::get_response() {
	//if (GET)
	//if (POST)
	//if (DELETE)
	const Method *m = new Get(_path);

	_response = m->get_header() + m->get_body();
	delete m;
	return _response;
}