#include "Delete.hpp"

Delete::Delete(std::string path, std::string content): Method(path, content) {}

Delete::~Delete() {}

void	Delete::execute_method() {
	if (std::remove(this->get_path().c_str()) != 0)
		error_throw("std::remove failed");
	set_statuscode(200);
	set_header(" " \
	+ int_to_str(get_status_code()) \
	+ " " \
	+ HttpStatusCode::get_error_msg(get_status_code()) \
	+ "\r\n\r\n");
}