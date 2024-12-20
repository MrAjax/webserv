#include "StatusSender.hpp"

StatusSender::StatusSender(int err, Server &serv, bool is_serv) {
	_status_code = 0;
	if (is_serv) {
		_server_path = serv.getRoot();
		_error_pages = serv.getLocationErrorPage();
		
		switch (err)
		{
		case 400:
			_response = _error_400();
			break;
		case 401:
			_response = _error_401();
			break;
		case 403:
			_response = _error_403();
			break;
		case 404:
			_response = _error_404();
			break;
		case 500:
			_response = _error_500();
			break;
		default:
			_response = _error_generic(err);
			break;
		}
	}
	else
		_response = _error_generic(err);
	server_log("Response status is ready", DEBUG);
}

StatusSender::~StatusSender() {}

std::string StatusSender::_error_400() {
    std::string	file_path(_server_path + _error_pages + "/" + "400.html");
	std::stringstream file_content;
    std::ifstream error_file(file_path.c_str());

    _status_code = 400;

    if (!error_file.is_open()) {
        server_log("Default 400 page not found", DEBUG);
        file_content << basic_page("400 Bad Request", "Your request is malformed or invalid.");
    }
    else {
        server_log("Found default page for 400 error", DEBUG);
        file_content << error_file.rdbuf();
        error_file.close();
        if (ContentTypeFinder::get_content_type(file_path).empty())
            error_throw("Server crashed", true);
    }
    _body = file_content.str();
    _header = build_header(_status_code, "text/html", _body.length(), "close");
    return _header + _body;
}

std::string StatusSender::_error_401() {
    std::string	file_path(_server_path + _error_pages + "/" + "401.html");
	std::stringstream file_content;
	std::ifstream error_file(file_path.c_str());

	_status_code = 401;

    if (!error_file.is_open()) {
        server_log("Default 401 page not found", DEBUG);
        file_content << basic_page("401 Unauthorized", "You must authenticate to access this resource.");
    }
    else {
        server_log("Found default page for 401 error", DEBUG);
        file_content << error_file.rdbuf();
        error_file.close();
        if (ContentTypeFinder::get_content_type(file_path).empty())
            error_throw("Server crashed", true);
    }
    _body = file_content.str();
    _header = build_header(_status_code, "text/html", _body.length(), "close");
	return _header + _body;
}


std::string	StatusSender::_error_403() {
    std::string	file_path(_server_path + _error_pages + "/" + "403.html");
	std::stringstream	file_content;
	std::ifstream		error_file(file_path.c_str());

	_status_code = 403;
	
	if (!error_file.is_open()) {
		server_log("Default 403 page not found", DEBUG);
		file_content << basic_page("403 Forbidden", \
		"You don't have permission to access this resource.");
	}
	else {
		server_log("Found default page for 403 error", DEBUG);
		file_content << error_file.rdbuf();
		error_file.close();
		if (ContentTypeFinder::get_content_type(file_path) != "text/html")
			error_throw("Server crashed", true);
	}
	_body = file_content.str();
	_header = build_header(_status_code, "text/html", _body.length(), "close");
	return _header + _body;
}

std::string	StatusSender::_error_404() {
    std::string	file_path(_server_path + _error_pages + "/" + "404.html");
	std::stringstream	file_content;
	std::ifstream		error_file(file_path.c_str());

	_status_code = 404;
	if (!error_file.is_open()) {
			server_log("Default 404 page not found", DEBUG);
			file_content << basic_page("404 - Page Not Found", \
			"The requested page could not be found.");
	}
	else {
		server_log("Found default page for 404 error", DEBUG);
		server_log(std::string(WHITEE) + "Copying file...", DEBUG);
		file_content << error_file.rdbuf();
		server_log(std::string(WHITEE) + "File copied", DEBUG);
		error_file.close();
		server_log(std::string(WHITEE) + "File closed", DEBUG);
		if (ContentTypeFinder::get_content_type(file_path).empty())
			return _error_500();
		server_log("Content type ok", DEBUG);
	}
	_body = file_content.str();
	_header = build_header(_status_code, "text/html", _body.length(), "close");
	return _header + _body;
}

std::string	StatusSender::_error_500() {
    std::string	file_path(_server_path + _error_pages + "/" + "500.html");
	std::stringstream	file_content;
	std::ifstream		error_file(file_path.c_str());

	_status_code = 500;
	
	if (!error_file.is_open()) {
		server_log("Default 500 page not found", DEBUG);
		file_content << basic_page("500 Internal Server Error", \
		"Sorry, something went wrong on our end. We're working to fix it. Please try again later.");
	}
	else {
		server_log("Found default page for 505 error", DEBUG);
		file_content << error_file.rdbuf();
		error_file.close();
		if (ContentTypeFinder::get_content_type(file_path).empty())
			error_throw("Server crashed", true);
	}
	_body = file_content.str();
	_header = build_header(_status_code, "text/html", _body.length(), "close");
	return _header + _body;
}

std::string	StatusSender::_error_generic(int err) {
	_body = basic_page(int_to_str(err) + " " + HttpStatusCode::get_error_msg(err), "");
	_header = build_header(err, "text/html", _body.length(), "close");
	return _header + _body;
}

std::string		StatusSender::_get_response() {return _response;}

std::string		StatusSender::send_status(int err, Server &serv, bool is_server) {
	StatusSender	status(err, serv, is_server);
	return status._get_response();
}
