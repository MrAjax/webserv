#ifndef UTILS_HPP
#define UTILS_HPP

#include "webserv.hpp"

std::string	int_to_str(int n) {
	std::stringstream	ss;

	ss << n;
	return ss.str();
}

#endif