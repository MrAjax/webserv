#include "Get.hpp"

Get::Get(std::string path, std::string content): Method(path, content) {}

Get::~Get() {}

static	bool	is_cgi(std::string file_name) {
	std::string	type;

	type = ContentTypeFinder::get_content_type(file_name);
	if (type != "cgi")
		return (false);
	return (true);
}

void	Get::execute_method(Server &serv) {
	std::string			file_name(this->get_path());
	std::ifstream		file_requested(file_name.c_str());
	std::stringstream	file_content;

	if (is_cgi(file_name)) {
		server_log("Found CGI", DEBUG);
		file_name.resize(file_name.size() - EXT_SIZE);
		set_statuscode(Cgi::exec_cgi(file_name));
	}
	else if (!file_requested.is_open()) {
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
