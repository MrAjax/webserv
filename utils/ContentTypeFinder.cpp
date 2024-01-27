#include "ContentTypeFinder.hpp"

ContentTypeFinder::ContentTypeFinder() {
	server_log(std::string(WHITEE) + "Opening server/MIME_types file", DEBUG);
	std::ifstream	mime_file("server/MIME_types");
	std::string		line;
	std::string		key;
	std::string		value;

	if (!mime_file.is_open())
		error_throw(std::string(WHITEE) + "Cannot open the MIME file", true);
	
	server_log(std::string(WHITEE) + "server/MIME_types file is ready", DEBUG);
	server_log(std::string(WHITEE) + "Reading server/MIME_types file...", DEBUG);
	while(std::getline(mime_file, line)) {
		std::stringstream	ss(line);
		ss >> key;
		ss >> value;
		_type_dictionnary.insert(std::make_pair(key, value));				
	}
	mime_file.close();
	server_log(std::string(WHITEE) + "Content Type Finder ready to use", DEBUG);
}

ContentTypeFinder::~ContentTypeFinder() {}

std::map<std::string, std::string>	ContentTypeFinder::_get_map() {
	return _type_dictionnary;
}

std::string	ContentTypeFinder::get_content_type(std::string path) {
	server_log("Finding content type for file: " + path, DEBUG);
	server_log(std::string(WHITEE) + "Setting up Content Type Finder...", DEBUG);

	std::map<std::string, std::string>				type_map;
	std::map<std::string, std::string>::iterator	content_type;
	ContentTypeFinder								Finder;
	size_t											extension_pos;
	std::string										extension;
	std::string										type;

	type_map = Finder._get_map();
	extension_pos = path.find('.', 0);
	while (path.find('.', extension_pos + 1) != std::string::npos)
		extension_pos = path.find('.', extension_pos + 1);

	if (extension_pos >= path.size()) {
		server_log(std::string(WHITEE) + path + " content type: " + type, DEBUG);
		return "text/plain";
	}
	extension = path.substr(extension_pos, path.size());
	server_log(std::string(WHITEE) + "File extension: " + extension, DEBUG);

/* 	for (std::map<std::string, std::string>::iterator it = type_map.begin(); it != type_map.end(); ++it) {
		if (it->first == extension) {
			server_log(std::string(WHITEE) + "Map key: " + it->first + ", Map value: " + it->second, DEBUG);
			break;
		}
	} */
	content_type = type_map.find(extension);
	if (content_type != type_map.end())
		type = content_type->second;
	else
		type = "text/plain";
	server_log(std::string(WHITEE) + path + " content type: " + type, DEBUG);
	return type;
}
