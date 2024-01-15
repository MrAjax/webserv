#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest &req) {
	// Recuperer ces infos de la classe Request
	(void)req;

//	_connfd			= req.getConnfd();
//	_contentType	= req.getContentType();
//	_input			= req.getInput();

	_method			= "GET";
	_method_code	= GET;
	_path			= "/";
//	_protocol = "HTTP/1.1";
//	_header = "Host";
//	_status_code = 200;
//	_status_msg = codeHttp.get_http_status_code().find(200)->second;
//	_response = _protocol + " " + _status_code + _status_msg + "\r\n" + _header;
}

HttpResponse::~HttpResponse() {}

void HttpResponse::sendhtml(int connfd, std::string ContentType, std::string input)
{
	(void)connfd;
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

	//std::cout << " === Response ===\n" << response << "\n";
	_response = response;
	//write(connfd, response.c_str(), response.length());
	}
	else{
		std::cerr << "Error HTML" << std::endl;
	}
}

std::string	HttpResponse::get_response() {
	Method *m;

	switch (_method_code)
	{
	case GET:
		m = new Get("index.html"); // _path a la place
		m->execute_method();
		break;
	case POST:
		m = new Post(_path);
		break;
	case DELETE:
		m = new Delete(_path);
		break;
	default:
		break;
	}

	_response = m->get_header() + m->get_body();
	
	delete m;
	return _response;
}
