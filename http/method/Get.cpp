#include "Get.hpp"

Get::Get(std::string path, std::string content): Method(path, content) {}

Get::~Get() {}

void	Get::execute_method() {
	std::string			file_name(this->get_path());
	std::ifstream		file_requested(file_name.c_str());
	std::stringstream	file_content;

	if (!file_requested.is_open())
		error_throw("Cannot open" + file_name);
	file_content << file_requested.rdbuf();
	file_requested.close();
	set_body(file_content.str());
	set_statuscode(200);
	set_header(" " \
	+ int_to_str(get_status_code()) \
	+ " " \
	+ HttpStatusCode::get_error_msg(get_status_code()) \
	+ "\r\n" \
	+ "Content-Type: " \
	+ get_content_type() \
	+ "\r\n" \
	+ "Content-Length: " \
	+ int_to_str(get_body().length()) \
	+ "\r\n\r\n");
}
