#include "utils/HttpStatusCode.hpp"
#include "HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "inc/webserv.hpp"

static	void	send_response(int connfd) {
	server_log("Activity detected", DEBUG);
	if (connfd < 0)
		throw error_throw("send response - main.cpp");
	server_log("connfd ready", DEBUG);
	server_log("Request received", INFO);
	server_log("Parsing Request...", DEBUG);
	server_log("Request is valid", DEBUG);
	HttpRequest 	Req(connfd);
	server_log(Req.getHeaderRequest() + "\n\n", DIALOG);
	server_log("Building Response..", DEBUG);
	HttpResponse	Rep(Req);
	std::string		response(Rep.get_response());
	server_log("Sending response...", DEBUG);
	write(connfd, response.c_str(), response.length());
	close(connfd);
	server_log("Response sent", INFO);
	server_log(Rep.get_header(), DIALOG);
}

static	void	open_connection(int &listenfd, struct sockaddr_in &servaddr) {
	server_log("Openning connection...", DEBUG);

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error_throw("open connection - socket creation - main.cpp");
	bzero (&servaddr, sizeof(servaddr));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);

	if ((bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0)
		throw error_throw("Port " + int_to_str(SERVER_PORT));
	if (listen(listenfd, 10) < 0)
		throw error_throw("listen error - open_connection - main.cpp");
		
	server_log("Connection established on port " + int_to_str(SERVER_PORT), INFO);
	std::cout << std::string(GREENN) + "PORT " << SERVER_PORT << ": Ready for connection\n" + std::string(ENDD);
}

static	void	init_log_file() {
	std::fstream log_file("server/log", std::ios::out | std::ios::trunc);
    
    if (!log_file.is_open())
		throw error_throw("log file cannot be created - init_log_file - main.cpp");
    log_file.close();
}

int main()
{
	int					listenfd;
	struct sockaddr_in	servaddr;
	uint8_t				recvline[MAXLINE + 1];
	struct pollfd 		pfds;

	try {
		init_log_file();
		server_log("SERVER STARTED", INFO);
		open_connection(listenfd, servaddr);
		pfds.fd = listenfd;
		pfds.events = POLLIN;
		for (;;) { /* Here is the main loop */
			if (poll(&pfds, 1, -1) == -1)
				error_throw("poll failure - main - main.cpp");
			if (pfds.revents & POLLIN) {
				if (pfds.fd == listenfd) {
					memset(recvline, 0, MAXLINE);
					send_response(accept(listenfd, (SA *) NULL, NULL));
				}
			}
		}
		close(listenfd);
		server_log("CONNECTION CLOSED", DIALOG);
		server_log("SERVER STOPPED", INFO);
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << "\n";
	}
}