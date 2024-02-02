#include "Get.hpp"

Get::Get(std::string path, std::string content): Method(path, content) {}

Get::~Get() {}

void	Get::execute_method(Server &serv) {
	std::string			file_name(this->get_path());
	std::ifstream		file_requested(file_name.c_str());
	std::stringstream	file_content;

	if (!file_requested.is_open()) {
		server_log("Cannot open " + file_name, ERROR);
		throw	StatusSender::send_status(404, serv);
	}
	else {
		server_log("Server successfully found " + file_name, DEBUG);
		file_content << file_requested.rdbuf();
		file_requested.close();
		set_statuscode(200);
	}
	set_body(file_content.str());
	set_header(build_header(get_status_code(), \
							get_content_type(), \
							get_body().length()));
}
