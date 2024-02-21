#include "signal.hpp"

void	sigint_handler(int signum)
{
	(void)signum;
	g_sig = 1;
	server_log("Signal caught", INFO);
	// server_log("Stopping server...", INFO);
	//throw std::runtime_error(std::string(WHITEE) + "\nStopping server..." + std::string(ENDD));
}

void	sigquit_handler(int signum)
{
	(void)signum;
	g_sig = 1;
	server_log("Signal caught", INFO);
	// server_log("Stopping server...", INFO);
	//throw std::runtime_error(std::string(WHITEE) + "\nStopping server..." + std::string(ENDD));
}
