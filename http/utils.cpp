#include "utils.hpp"

std::string	int_to_str(int n) {
	std::stringstream	ss;

	ss << n;
	return ss.str();
}