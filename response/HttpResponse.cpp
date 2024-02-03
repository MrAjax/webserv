#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest &req, Server &serv):  _method(req.getMethod()), \
										_method_code(_method.length()), \
										_path(req.getPath()), \
										_server_path(serv.getRoot()), \
										_index_list(serv.getIndex()), \
										_body_request(req.getBodyRequest()), \
										_status_code(/* req.getStatusCode() */202), \
										_is_cgi(0) {
	if (_status_code != 202)
		throw StatusSender::send_status(_status_code, serv);
	if (is_cgi(_path)) {
		server_log("Found CGI", DEBUG);
		_is_cgi = 1;
		_path.resize(_path.size() - EXT_SIZE);
		server_log("Cgi file: " + _path, DEBUG);
		_status_code = Cgi::exec_cgi(_path, _body);
		if (_status_code != 200)
			throw StatusSender::send_status(_status_code, serv);
	}
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
	if (_is_cgi) {
		_header = build_header(_status_code, "text/html", _body.size());
		_response = _header + _body;
		return _response;
	}
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
