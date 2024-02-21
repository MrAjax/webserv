#include "Post.hpp"

Post::Post(const std::string &path, const std::string &raw_path, const std::string &root, const std::string &content, const std::string &connection_status, std::string &body_request): Method(path, raw_path, root, content, connection_status, body_request) {}

Post::~Post() {}

std::string	Post::_guess_mime_type(std::string &body) {
	std::string	pattern;
	std::string	multipart_form = "------";
	std::string	field("filename=\"");
	std::string	value;
	size_t		pos = body.find(field);

	server_log("BODY TO POST: " + body, INFO);
	if (get_content_type().find("multipart/form-data") == std::string::npos || pos == std::string::npos) {
		server_log("not a multipart", DEBUG);
		_set_post_path(get_raw_path());
		return "_data.csv";
	}
	body = body.substr(pos + field.size(), body.size() - pos - field.size());
	pos = body.find_first_of("\"");
	if (pos == std::string::npos)
		return ".txt";
	value = body.substr(0, pos);
	if (value.empty()) {
		server_log("No file to upload", DEBUG);
		set_path("");
		return "";
	}
	server_log("File name: " + value, DEBUG);
	server_log("Retrieving the body", DEBUG);
	for (int i = 0; i < 3 && pos < body.size(); ++i) {
		pos = body.find_first_of('\n');
		if (pos == std::string::npos)
			return ".txt";
		++pos;
		body = body.substr(pos, body.size() - pos);
	}
	pos = body.find(multipart_form);
	if (pos != std::string::npos)
		body = body.substr(0, pos + 1);
	for (int i = 0; i < 3 && !body.empty(); ++i) {
		body.erase(body.end() - 1);
	}
	server_log("set_path = value [" + value + "] + path [" + get_path()+ "]", DEBUG);
	set_path(get_path() + value);
	return "";
}

void	Post::_set_post_path(std::string path) {
	server_log("Preparing post file...", DEBUG);
	std::string::reverse_iterator	it = path.rbegin();
	std::string	post_path;

	while (it != path.rend()) {
		if (*it == '.') {
			post_path = path.substr(0, it.base() - path.begin() - 1);
			break;
		}
		it++;
	}
	server_log("get_root() = " + get_root(), DEBUG);
	server_log("post_path = " + post_path, DEBUG);
	set_path(get_root() + post_path);
}

static	void	post_encoded_text(std::string &query_string, std::fstream &post_file) {
	std::istringstream			query_stream(query_string);
	std::string					key, value, token;
	std::vector<std::string>	values;

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
}

int	checkCreate(std::string path)
{
	return (access(path.c_str(), F_OK));
}

int createDir(std::string path)
{
	return (mkdir(path.c_str(), 0777));
}

void	Post::_fill_post_file(Server &serv, std::string body) {


	std::string		ext = _guess_mime_type(body);
	std::string		path = get_path() + ext;
	std::fstream	post_file;

	if (path.empty())
		return;
	server_log("Post file location: " + path, DEBUG);
	if (get_content_type().find("multipart/form-data") != std::string::npos)
		post_file.open(path.c_str(), std::ios::out | std::ios::trunc);
	else
		post_file.open(path.c_str(), std::ios::out | std::ios::app);

	if (!post_file.is_open()) {
		server_log("Cannot open post file - Post.cpp", ERROR);
		throw StatusSender::send_status(500, serv, true);
	}
	if (get_content_type() == "application/x-www-form-urlencoded")
		post_encoded_text(body, post_file);
	else if (get_content_type().find("multipart/form-data") != std::string::npos) {
		post_file << body;
		if (!body.empty())
			post_file << "\n";
	}
	else {
		post_file << get_body_request();
		post_file << "\n";
	}
	post_file.close();
}

void	Post::execute_method(Server &serv) {
	server_log("current path: " + get_path(), DEBUG);
	server_log("content type: " + get_content_type(), DEBUG);
	std::string	redirect_path = get_raw_path();
	_fill_post_file(serv, get_body_request());
	set_statuscode(303);
	set_header(" " \
	+ int_to_str(get_status_code()) + " " + HttpStatusCode::get_error_msg(get_status_code()) \
	+ "\r\nLocation: " + redirect_path \
	+ "\r\nContent-Length: 0" \
	+ "\r\nConnection: " + this->get_connection_status() \
	+ "\r\n\r\n");
}
