#include "signal.hpp"

void	sigint_handler(int signum)
{
	(void)signum;
	server_log("Stopping server...", INFO);
	throw std::runtime_error("\nStopping server...");
}

void	sigquit_handler(int signum)
{
	(void)signum;
	server_log("Stopping server...", INFO);
	throw std::runtime_error("\nStopping server...");
}
