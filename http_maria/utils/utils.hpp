#ifndef UTILS_HPP
#define UTILS_HPP

#include "../inc/webserv.hpp"

std::string			int_to_str(int n);
std::string			getTimestamp(void);
void				server_log(std::string content, int log_level);
std::runtime_error	error_throw(std::string description);
std::string			website(std::string path);

#endif