#ifndef UTILS_HPP
#define UTILS_HPP

#include "../inc/webserv.hpp"

std::string			int_to_str(int n);
std::string			getTimestamp(void);
void				server_log(std::string content, int log_level);
std::runtime_error	error_throw(std::string description);
std::string			website(std::string path);
std::string			build_header(int status_code, std::string content_type, size_t body_len);
std::string 		basic_page(std::string title, std::string p);

#endif
