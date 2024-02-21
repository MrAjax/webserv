#include "LoopUtils.hpp"

//----------- INIT SERVER -----------------

void	init_log_file(void) {
	std::fstream log_file(LOG_FILE, std::ios::out | std::ios::trunc);
    
    if (!log_file.is_open())
		throw error_throw("log file cannot be created - init_log_file - main.cpp", true);
    log_file.close();
}

void	init_server(void) {
	g_sig = 0;
	signal(SIGINT, &sigint_handler);
	signal(SIGQUIT, &sigquit_handler);
	init_log_file();
	server_log("SERVER STARTED", INFO);
}

//----------- ADDING CLIENT -----------------

bool isListener(int fd, std::vector<Server> servers) {
	for (size_t i = 0; i < servers.size(); i++) {
		if (fd == servers[i].getSocketfd())
			return true;	
	}
	return false;
}


