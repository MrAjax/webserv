#include "HttpResponse.hpp"

std::string	HttpResponse::_find_index_path(Server &serv) {
	std::string	index_path;
	size_t 		i;
	size_t 		j;

	server_log("Get Request for / --> replace by index.html", DEBUG);
	server_log(std::string(WHITEE) + "Index list: " + _index_list, DEBUG);
	for (i = 0; i < _index_list.size(); i++) {
		server_log(std::string(WHITEE) + "Searching index...", DEBUG);
		while (i < _index_list.size() && _index_list[i] == ' ')
			i++;
		j = 0;
		while (j + i < _index_list.size() && _index_list[j + i] != ' ')
			j++;
		std::string		file_to_test(_server_path + _index_list.substr(i, j));
		std::ifstream	index_file(file_to_test.c_str());
		server_log(std::string(WHITEE) + "Trying file: " + file_to_test, DEBUG);
		if (index_file.is_open()) {
			index_path = _server_path + _index_list.substr(i, j);
			index_file.close();
			break;
		}
		i += j;
	}
	if (i >= _index_list.size()) {
		server_log("Index file not found", ERROR);
		throw StatusSender::send_status(404, serv);
	}
	server_log("Found index file: " + index_path, DEBUG);
	return index_path;
}

HttpResponse::HttpResponse(HttpRequest &req, Server &serv):  _method(req.getMethod()), \
										_method_code(_method.length()), \
										_path(req.getPath()), \
										_server_path(serv.getRoot()), \
										_index_list(serv.getIndex()), \
										_status_code(202 /* should take it from the req in case of an error */) {
	if (_status_code != 202)
		throw StatusSender::send_status(_status_code, serv);

	// if (_path == "/")
	// 	_path = _find_index_path(serv);
	// else
	// 	_path = _server_path + _path;

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
