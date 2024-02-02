#include "Cgi.hpp"

Cgi::Cgi() {}

Cgi::~Cgi() {}

int	Cgi::exec_cgi(std::string path) {
	int	fd[2];
	if (access(path.c_str(), F_OK) < 0) {
		server_log("Cgi file not found:" + path, DEBUG);
		return (404);
	}
	else if (access(path.c_str(), X_OK) < 0) {
		server_log("Cannot execute cgi file:" + path, DEBUG);
		return(403);	
	}
	if (pipe(fd) < 0) {
		server_log("Pipe error in cgi", DEBUG);
		return 500;
	}
	close(fd[0]);
	close(fd[1]);
	return 0;
}
