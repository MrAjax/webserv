#include "Cgi.hpp"

extern char **environ;

Cgi::Cgi() {}

Cgi::~Cgi() {}

int	Cgi::exec_cgi(std::string path) {
	if (access(path.c_str(), F_OK) < 0) {
		server_log("Cgi file not found:" + path, DEBUG);
		return (404);
	}
	else if (access(path.c_str(), X_OK) < 0) {
		server_log("Cannot execute cgi file:" + path, DEBUG);
		return(403);	
	}
	int pid = fork();
	if (pid < 0) {
		server_log("Fork error in cgi", DEBUG);
		return (500);
	}
	else if (pid == 0) {
		char	*arg[] = {const_cast<char*>(path.c_str()), NULL};
		execve(path.c_str(), arg, environ);
	}
	else {
		int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            return 200;
		else {
            server_log("CGI script execution failed: " + path, ERROR);
            return 500;
        }
	}
	return 200;
}
