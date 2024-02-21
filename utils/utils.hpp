#ifndef UTILS_HPP
#define UTILS_HPP

#include "../inc/webserv.hpp"
#include "ContentTypeFinder.hpp"
#include "HttpStatusCode.hpp"

#define START  		0
#define END 		1
#define ALL 		2


std::string			int_to_str(long int n);
std::string			getTimestamp(void);
void				server_log(std::string content, int log_level);
std::runtime_error	error_throw(std::string description, bool errno_need);
std::string			build_header(int status_code, std::string content_type, size_t body_len, std::string connection_status);
std::string 		basic_page(std::string title, std::string p);
bool				is_cgi(std::string file_name);
std::string	        trimString(std::string str, std::string const &toTrim, int STARTorEND);
ssize_t             strToSsize_t(std::string const &toConvert);
std::size_t         findLine(std::string &header, std::string &line, std::string &delimiteur);
std::string         uploadPrint(std::size_t current, std::size_t total);

#endif
