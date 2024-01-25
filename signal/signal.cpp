#include "signal.hpp"

void	sigint_handler(int signum)
{
	(void)signum;
	server_log("Signal caught", DEBUG);
	server_log("Stopping server...", INFO);
	throw std::runtime_error(std::string(WHITEE) + "\nStopping server..." + std::string(ENDD));
}

void	sigquit_handler(int signum)
{
	(void)signum;
	server_log("Signal caught", DEBUG);
	server_log("Stopping server...", INFO);
	throw std::runtime_error(std::string(WHITEE) + "\nStopping server..." + std::string(ENDD));
}