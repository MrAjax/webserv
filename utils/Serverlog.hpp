#ifndef SERVERLOG_HPP
# define SERVERLOG_HPP

#include "../inc/webserv.hpp"
#include "utils.hpp"

class Serverlog {
    static	std::fstream	_log_file;
public:
    static  void    open_file();
    static  void	server_log(std::string content, int log_level);
    static  void    close_file();
};

#endif