#include "Post.hpp"

Post::Post(std::string path, std::string content, std::string body_request): Method(path, content, body_request) {}

Post::~Post() {}

std::string	set_post_path(std::string path) {
	server_log("Preparing post file...", DEBUG);
	std::string::reverse_iterator	it = path.rbegin();
	std::string	post_path;

	while (it != path.rend()) {
		if (*it == '.') {
			post_path = path.substr(0, it.base() - path.begin() - 1);
			server_log("Post file location: " + post_path + "_data.csv", DEBUG);
			break;
		}
		it++;
	}
	return post_path + "_data.csv";
}

void	Post::execute_method(Server &serv) {
	// TODO generaliser le path du post file suivant le contexte
	// TODO Status 200: A description of the result of the action is transmitted to the message body.
	std::fstream	post_file(set_post_path(this->get_path()).c_str(), std::ios::out | std::ios::app);

	if (!post_file.is_open()) {
		server_log("Cannot open post file - Post.cpp", ERROR);
		throw StatusSender::send_status(500, serv);
	}
	post_file << this->get_body_request();
	post_file.close();
	set_statuscode(200);
	set_header(build_header(get_status_code(), \
							"text/plain", \
							get_body().length()));
}