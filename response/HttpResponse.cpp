#include "HttpResponse.hpp"

void	check_rights(std::string path, int method_code) {
	(void)path;
	(void)method_code;
	// TODO

	// Check for the rights of the request	
	// Not the right rights ? --> Send Status code 403 + associated page
	// Check if the user is logged (user or admin)
	// If the request wants to access a welcome page for example and there is no authentication
	// then, send Status code 401 - Unauthorized
}

HttpResponse::HttpResponse(HttpRequest &req, Server &serv):  _method(req.getMethod()), \
										_method_code(_method.length()), \
										_path(req.getPath()), \
										_server_name(serv.getServerName()), \
										_status_code(202 /* should take it from the req in case of an error */) {
	if (_status_code != 202)
		throw StatusSender::send_status(_status_code, serv);

	if (_path == "/") {
		server_log(std::string(WHITEE) + "Get Request for / --> replace by index.html", DEBUG);
		_path = _server_name + serv.getIndex();
		//_path = _server_name + "/index/index.html";
	}
	else
		_path = _server_name + _path;

	check_rights(_path, _method_code);	
	if (_method_code == POST || _method_code == DELETE)
		_body_request = req.getBodyRequest();

	server_log(std::string(WHITEE) + "method = " + _method, DEBUG);
	server_log(std::string(WHITEE) + "method code = " + int_to_str(_method_code), DEBUG);
	server_log(std::string(WHITEE) + "path = " + _path, DEBUG);
}

HttpResponse::~HttpResponse() {}

Method	*HttpResponse::_execute_method(int method_code,Server &serv) {
	Method	*m;

	m = NULL;
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
	m->execute_method(serv);
	return m;
}

std::string	HttpResponse::get_response(Server &serv) {
	Method *m;
	_contentType = ContentTypeFinder::get_content_type(_path);
	if (_contentType.empty())
		throw StatusSender::send_status(404, serv);
	m = _execute_method(_method_code, serv);
	_header = m->get_header();
	_response = _header + m->get_body();
	//std::cout << std::string(GREENN) + _header << "\n";
	delete m;
	server_log("Response ready to send", DEBUG);
	return _response;
}

std::string	HttpResponse::get_header() {return _header;}
