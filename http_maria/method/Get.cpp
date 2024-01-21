#include "Get.hpp"

Get::Get(std::string path, std::string content): Method(path, content) {}

Get::~Get() {}

void	Get::execute_method() {
	std::string			file_name(this->get_path());
	std::ifstream		file_requested(file_name.c_str());
	std::stringstream	file_content;

	if (!file_requested.is_open()) {
		set_statuscode(404);
		server_log("Cannot open " + file_name, ERROR);
		std::ifstream	error_file(website("/errorpages/404.html").c_str());
		if (!error_file.is_open()) {
			server_log("Default 404 page not found", DEBUG);
			file_content	<< "<!DOCTYPE html>\n" \
							<< "<html lang=\"en\">\n" \
							<< "<head>\n" \
							<< "    <meta charset=\"UTF-8\">\n" \
							<< "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n" \
							<< "    <title>404 - Page Not Found</title>\n" \
							<< "</head>\n" \
							<< "<body>\n" \
							<< "    <header>\n" \
							<< "        <h1>404 - Page Nooooot Found</h1>\n" \
							<< "    </header>\n"
							<< "</body>\n"
							<< "</html>\n";
		}
		else {
			server_log("Found default page for 404 error", DEBUG);
			file_content << error_file.rdbuf();
			error_file.close();
		}
	}
	else {
		server_log("Server successfully found " + file_name, DEBUG);
		file_content << file_requested.rdbuf();
		file_requested.close();
		set_statuscode(200);
	}
	set_body(file_content.str());
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
