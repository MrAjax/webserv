#include "Get.hpp"

Get::Get(std::string path /* std::string type */): Method(path) {
	/* _ressource_type = _type (for exemple text, image, etc.) */
}

Get::~Get() {}

void	Get::get_text() {
	std::ifstream		file_requested(this->get_path().c_str());
	std::stringstream	file_content;

	if (!file_requested.is_open())
		throw std::runtime_error("ERROR: Cannot open the requested ressource\n");
	file_content << file_requested.rdbuf();
	file_requested.close();
	set_body(file_content.str());
	// Should we keep the following here ??
	// Add a way to set the content type inside of the Method class
	// Maybe change this using va_arg or maybe not
	set_statuscode(200);
	set_header(" " \
	+ int_to_str(get_status_code()) \
	+ " " \
	+ HttpStatusCode::get_error_msg(get_status_code()) \
	+ "\r\n" \
	+ "Content-Type: text/html\r\n" \
	+ "Content-Length: " \
	+ int_to_str(get_body().length()) \
	+ "\r\n\r\n");
}

void	Get::execute_method() {
	std::string	header;

	/* if (_type == text) */
	get_text();
}