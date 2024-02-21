#include "Delete.hpp"

Delete::Delete(const std::string &path, const std::string &content, const std::string &connection_status): Method(path, content, connection_status) {}

Delete::~Delete() {}

void	Delete::remove_file(Server &serv) {
	(void)serv;
	server_log("Remove entire file", DEBUG);
	if (std::remove(this->get_path().c_str()) != 0) {
		server_log("std::remove failed - Delete.cpp", ERROR);
		throw StatusSender::send_status(500, serv, true);
	}
}

void	Delete::execute_method(Server &serv) {
	server_log("Delete", DEBUG);
	remove_file(serv);
	set_statuscode(200);
	set_header("HTTP/1.1 " + int_to_str(get_status_code()) + " " + HttpStatusCode::get_error_msg(get_status_code()) \
	+ "\r\nConnection: " + get_connection_status() \
	+ "\r\nContent-Length: 0"
	+ "\r\n\r\n");
}
