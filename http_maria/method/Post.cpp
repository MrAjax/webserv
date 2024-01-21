#include "Post.hpp"

Post::Post(std::string path, std::string content, std::string body_request): Method(path, content, body_request) {}

Post::~Post() {}

void	Post::execute_method() {
	// TODO generaliser le path du post file suivant le contexte
	// TODO Status 200: A description of the result of the action is transmitted to the message body.
	std::fstream	post_file("data/ground_beetle_club.csv");

	if (!post_file.is_open())
		error_throw("Cannot open post file");
	post_file << this->get_body_request();
	post_file.close();
	set_statuscode(200);
	set_header(" " \
	+ int_to_str(get_status_code()) \
	+ " " \
	+ HttpStatusCode::get_error_msg(get_status_code()) \
	+ "\r\n\r\n");
}