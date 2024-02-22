#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest &req, Server &serv):  _method(req.getMethod()), \
										_method_code(_method.length()), \
										_path(req.getPath()), \
										_raw_path(req.getBrutPath()), \
										_server_path(serv.getRoot()), \
										_index_list(serv.getIndex()), \
										_body_request(req.getBodyRequest()), \
										_status_code(req.getStatusCode()), \
										_is_cgi(req.getIsCgi()), \
										_cookie(req.getCookie()), \
										_connection_status(req.getConnection()), \
										_contentType(req.getContentType()) {
	if (_status_code != 202)
		throw StatusSender::send_status(_status_code, serv, true);
	if (_is_cgi) {
		server_log("Found CGI", DEBUG);
		server_log("Cgi file: " + _path, DEBUG);
		_status_code = Cgi::exec_cgi(_path, _response, _body_request, _cookie);
		if (_status_code != 200)
			throw StatusSender::send_status(_status_code, serv, true);
		else if (_response.empty() || _response[0] == 'X')
			throw StatusSender::send_status(400, serv, true);
	}
	server_log(std::string(WHITEE) + "method = " + _method, DEBUG);
	server_log(std::string(WHITEE) + "method code = " + int_to_str(_method_code), DEBUG);
	server_log(std::string(WHITEE) + "path = " + _path, DEBUG);
	server_log(std::string(WHITEE) + "serv name " + serv.getServerName(), DEBUG);


	if (_method_code == GET && !serv.getIsAllowed())
	{
		server_log("Method GET not allowed", ERROR);
		throw StatusSender::send_status(405, serv, true);
	}
	if (_method_code == POST && !serv.postIsAllowed())
	{
		server_log("Method POST not allowed", ERROR);
		throw StatusSender::send_status(405, serv, true);
	}
	if (_method_code == DELETE && !serv.deleteIsAllowed())
	{
		server_log("Method DELETE not allowed", ERROR);
		throw StatusSender::send_status(405, serv, true);
	}
}

HttpResponse::~HttpResponse() {}

Method	*HttpResponse::_execute_method(int method_code, Server &serv) {
	Method	*m;

	m = NULL;
	switch (method_code)
	{
	case GET:
		m = new Get(_path, _contentType, _connection_status);
		break;
	case POST:
		m = new Post(_path, _raw_path, _server_path, _contentType, _connection_status, _body_request);
		break;
	case DELETE:
		m = new Delete(_path, _contentType, _connection_status);
		break;
	default:
		break;
	}
	try {
		m->execute_method(serv);
	}
	catch (std::string &status_error) {
		delete m;
		throw status_error;
	}
	return m;
}

std::string	HttpResponse::get_response(Server &serv) {
	if (_is_cgi)
		return _response;
	Method *m;
	if (_method_code == DELETE && _contentType.empty())
		_contentType = "text/html";
	else if (_contentType.empty())
		_contentType = ContentTypeFinder::get_content_type(_path);
	if (_contentType.empty())
		throw StatusSender::send_status(404, serv, true);
	m = _execute_method(_method_code, serv);
	_header = m->get_header();
	_response = _header + m->get_body();
	delete m;
	server_log("Response ready to send", DEBUG);
	return _response;
}

std::string	HttpResponse::get_header() {return _header;}
