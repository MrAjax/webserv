#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest &req) {
	_method			= req.getMethod();
	_method_code	= _method.length();
	_path			= req.getPath();

	if (_path == "/" && _method_code == GET) {
		server_log(std::string(WHITEE) + "Get Request for / --> replace by index.html", DEBUG);
		_path = website("/index/index.html");
	}
	else if (_path == "/") {
		_status_code = 205;
		_status_msg = HttpStatusCode::get_error_msg(_status_code);
		_response = "HTTP/1.1 " + int_to_str(_status_code) \
				  + " " + _status_msg + "\r\n\r\n";
				  // TODO implementer les pages erreur directement dans les methodes
	}
	else
		_path = website(_path);
	
	if (_method_code == POST)
		_body_request = req.getBodyRequest();

	server_log(std::string(WHITEE) + "method = " + _method, DEBUG);
	server_log(std::string(WHITEE) + "method code = " + int_to_str(_method_code), DEBUG);
	server_log(std::string(WHITEE) + "path = " + _path, DEBUG);
}

HttpResponse::~HttpResponse() {}

std::string	HttpResponse::get_response() {
	Method *m;

	_contentType = ContentTypeFinder::get_content_type(_path);
	// TODO --> essayer un fichier avec extension inconnue

	switch (_method_code)
	{
	case GET:
		m = new Get(_path, _contentType);
		m->execute_method();
		break;
	case POST:
		m = new Post(_path, _contentType, _body_request);
		break;
	case DELETE:
		m = new Delete(_path, _contentType);
		break;
	default:
		break;
	}

	_header = m->get_header();
	_response = _header + m->get_body();	
	//std::cout << std::string(GREENN) + m->get_header() << "\n";
	delete m;
	server_log("Response ready to send", DEBUG);
	return _response;
}

std::string	HttpResponse::get_header() {
	return _header;
}