#include "Post.hpp"

Post::Post(std::string path, std::string content, std::string body_request, std::string connection_status): Method(path, content, body_request, connection_status) {}

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

void	Post::fill_post_file(Server &serv, std::string query_string) {
	std::string					path = set_post_path(this->get_path());
	std::fstream				post_file(path.c_str(), std::ios::out | std::ios::app);
	std::string					key, value, token;
	std::istringstream			query_stream(query_string);
	std::vector<std::string>	values;

	if (!post_file.is_open()) {
		server_log("Cannot open post file - Post.cpp", ERROR);
		throw StatusSender::send_status(500, serv, true);
	}
	while (getline(query_stream, token, '&')) {
		std::istringstream	token_stream(token);
		if (getline(token_stream, key, '=') && getline(token_stream, value))
			values.push_back(value);
	}
	for (size_t i = 0; i < values.size(); i++) {
		post_file << values[i];
		if (i < values.size() - 1)
			post_file << ",";
	}
	post_file << "\n";
	post_file.close();
}

void	Post::execute_method(Server &serv) {
	fill_post_file(serv, this->get_body_request());
	set_statuscode(303);
	set_header(" " \
	+ int_to_str(get_status_code()) + " " + HttpStatusCode::get_error_msg(get_status_code()) \
	+ "\r\nLocation: " + this->get_path().substr(serv.getRoot().length(), this->get_path().length() - serv.getRoot().length()) \
	+ "\r\nContent-Length: 0" \
	+ "\r\nConnection: " + this->get_connection_status() \
	+ "\r\n\r\n");
}
