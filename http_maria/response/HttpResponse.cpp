#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest &req):  _method(req.getMethod()), \
										_method_code(_method.length()), \
										_path(req.getPath()), \
										_status_code(202 /* should take it from the req in case of an error */) {
	if (_status_code != 202)
		throw HttpError(_status_code);
	
	if (_path == "/") {
		server_log(std::string(WHITEE) + "Get Request for / --> replace by index.html", DEBUG);
		_path = website("/index/index.html");
	}
	else
		_path = (website(_path));
	
	if (_method_code == POST || _method_code == DELETE)
		_body_request = req.getBodyRequest();

	server_log(std::string(WHITEE) + "method = " + _method, DEBUG);
	server_log(std::string(WHITEE) + "method code = " + int_to_str(_method_code), DEBUG);
	server_log(std::string(WHITEE) + "path = " + _path, DEBUG);
}

HttpResponse::~HttpResponse() {}

Method	*HttpResponse::_execute_method(int method_code) {
	Method	*m;

	switch (method_code)
	{
	case GET:
		m = new Get(_path, _contentType);
		break;
	case POST:
		m = new Post(_path, _contentType, _body_request);
		break;
	case DELETE:
		m = new Delete(_path, _contentType); // TODO a aussi besoin du body_request
		break;
	default:
		break;
	}
	m->execute_method();
	return m;
}

std::string	HttpResponse::get_response() {
	Method *m;
	_contentType = ContentTypeFinder::get_content_type(_path);
	// TODO --> essayer un fichier avec extension inconnue
//	try {
		m = _execute_method(_method_code);
//	}
//	catch (HttpError &error) {
		// should we delete m ?? -> TODO
//		delete m;
		// If we don't need to delete m, we don't need to try and catch
//		throw error;
//	}
	_header = m->get_header();
	_response = _header + m->get_body();
	//std::cout << std::string(GREENN) + _header << "\n";
	delete m;
	server_log("Response ready to send", DEBUG);
	return _response;
}

std::string	HttpResponse::get_header() {return _header;}