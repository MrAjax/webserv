#ifndef SIGNAL_HPP
# define SIGNAL_HPP

#include "../inc/webserv.hpp"
#include "../utils/utils.hpp"

void	sigint_handler(int signum);
void	sigquit_handler(int signum);

#endif