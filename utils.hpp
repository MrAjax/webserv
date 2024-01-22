#ifndef UTILS_HPP
#define UTILS_HPP
#include "parsing.hpp"
enum {NOLOG, ERROR, DIALOG, INFO, DEBUG};
#define LOGLVL DEBUG
#define GREENN "\033[32;1m"
#define YELLOWW "\033[38;5;227;1m"
#define REDD "\033[31;1m"
#define BLUEE "\033[36;1m"
#define WHITEE "\033[37;1m"
#define ENDD "\033[0m"

std::string			int_to_str(int n);
std::string			getTimestamp(void);
void				server_log(std::string content, int log_level);
std::runtime_error	error_throw(std::string description);

#endif
