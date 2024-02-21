#include "Get.hpp"

Get::Get(std::string path, std::string content, std::string connection_status): Method(path, content, connection_status) {}

Get::~Get() {}

void	Get::execute_method(Server &serv) {
	std::string			file_name(this->get_path());
	std::ifstream		file_requested(file_name.c_str());
	std::stringstream	file_content;
	struct stat			stat_struct;

	if (!file_requested.is_open()) {
		server_log("Cannot open " + file_name, ERROR);
		throw	StatusSender::send_status(404, serv, true);
	}
	else if (stat(file_name.c_str(), &stat_struct) == 0 && S_ISDIR(stat_struct.st_mode)) {
		file_requested.close();
		server_log("Cannot open " + file_name, ERROR);
		throw	StatusSender::send_status(404, serv, true);
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
							get_body().length(), \
							get_connection_status()));
}
