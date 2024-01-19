#include "ContentTypeFinder.hpp"

/* void print_map(const std::map<std::string, std::string>& myMap) {
    for (std::map<std::string, std::string>::const_iterator it = myMap.begin(); it != myMap.end(); ++it) {
        const std::string& key = it->first;
        const std::string& value = it->second;

        std::cout << "Key: " << key << ", Value: " << value << std::endl;
    }
} */

ContentTypeFinder::ContentTypeFinder() {
	std::ifstream	mime_file("httpconfig/MIME_types");
	std::string		line;
	std::string		key;
	std::string		value;

	if (!mime_file.is_open())
		throw std::runtime_error("ERROR: Cannot open the MIME file\n");
	
	while(std::getline(mime_file, line)) {
		std::stringstream	ss(line);
		ss >> key;
		ss >> value;
		_type_dictionnary.insert(std::make_pair(key, value));				
	}
//	print_map(_type_dictionnary);
	mime_file.close();
}

ContentTypeFinder::~ContentTypeFinder() {}

std::map<std::string, std::string>	ContentTypeFinder::get_map() {
	return _type_dictionnary;
}

std::string	ContentTypeFinder::get_content_type(std::string path) {
	ContentTypeFinder	Finder;
	size_t				ext_pos;

	ext_pos = path.find('.', 0);

	if (ext_pos >= path.size())
		return "";

	return Finder.get_map().find(path.substr(ext_pos, path.size()))->second;
}
