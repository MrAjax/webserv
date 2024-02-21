#include "Cgi.hpp"

extern char **environ;

Cgi::Cgi() {}

Cgi::~Cgi() {}

static	int	check_file_permission(std::string path) {
	server_log("Checking for the rights", DEBUG);
	if (access(path.c_str(), F_OK) < 0) {
		server_log("Cgi file not found:" + path, ERROR);
		return (404);
	}
	else if (access(path.c_str(), X_OK) < 0) {
		server_log("Cannot execute cgi file:" + path, DEBUG);
		return(403);
	}
	return (0);
}

int	child_process(int *fd, std::string &path) {
	server_log(std::string(WHITEE) + "Child process", DEBUG);
	char	*arg[] = {const_cast<char*>(path.c_str()), NULL};
	close(fd[0]);
	if (dup2(fd[1], STDOUT_FILENO) < 0) {
		server_log(std::string(WHITEE) + "dup2 failed in child process", DEBUG);
		return (500);
	}
	close(fd[1]);
	server_log(std::string(WHITEE) + "Execution", DEBUG);
	execve(path.c_str(), arg, environ);
	return (500);
}

int	Cgi::exec_cgi(std::string &path, std::string &output, std::string &input, std::string &cookie) {
	server_log("Executing script...", DEBUG);
	int	fd[2];
	int	status = check_file_permission(path);
	if (status)
		return (status);
	if (pipe(fd) < 0) {
		server_log("pipe error in cgi execution", ERROR);
		return (500);
	}
	server_log("Add QUERY_STRING to environ", DEBUG);
	if (setenv("QUERY_STRING", input.c_str(), 1) || setenv("COOKIE_ID", cookie.c_str(), 1)) {
		server_log("setenv error in cgi management", ERROR);
		return (500);
	}
	server_log("Fork process", DEBUG);
	int pid = fork();
	if (pid < 0) {
		server_log("Fork error in cgi", ERROR);
		return (500);
	}
	else if (pid == 0)
		return child_process(fd, path);
	else {
		server_log("Parent process", DEBUG);
		close(fd[1]);
		ssize_t	bytes_read;
		char	buffer[4096];
		while ((bytes_read = read(fd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytes_read] = '\0';
			output += buffer;
		}
		close(fd[0]);
        waitpid(pid, &status, 0);
		if (bytes_read < 0) {
			server_log("read error in cgi", ERROR);
			return 500;
		}
		server_log("child finished", DEBUG);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
			server_log("Cgi script execution SUCCESS", DEBUG);
            return 200;
		}
		else {
            server_log("CGI script execution failed: " + path, ERROR);
            return 400;
        }
	}
	return 200;
}
