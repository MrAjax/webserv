#include "Delete.hpp"

Delete::Delete(std::string path, std::string content): Method(path, content) {}

Delete::~Delete() {}

void	Delete::execute_method() {
	if (std::remove(this->get_path().c_str()) != 0)
		throw std::runtime_error("std::remove failed\n");
}