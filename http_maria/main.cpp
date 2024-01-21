#include "utils/HttpStatusCode.hpp"
#include "HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "inc/webserv.hpp"
#include "signal/signal.hpp"

volatile int	g_sig;

static	void	send_response(int connfd) {
	server_log("Activity detected", DEBUG);
	if (connfd < 0)
		throw error_throw("send response - main.cpp");
	server_log("connfd ready", DEBUG);
	server_log("Request received", INFO);
	HttpRequest 	Req(connfd);
	server_log("Parsing Request...", DEBUG);
	std::string	request_header = Req.getHeaderRequest();
	if (request_header.empty()) {
		server_log("Unsupported request type", ERROR);
		// Renvoyer erreur appropriee et page erreur associee
		// Construire reponse erreur
		// continuer (supprimer le return)
		throw std::runtime_error(std::string(REDD) + "Unsupported request type" + std::string(ENDD));
	}
	server_log("Request is valid", DEBUG);
	server_log(request_header + "\n\n", DIALOG);
	server_log("Building Response...", DEBUG);
	HttpResponse	Rep(Req);
	std::string		response(Rep.get_response());
	server_log("Sending response...", DEBUG);
	write(connfd, response.c_str(), response.length());
	server_log("Response sent", INFO);
	server_log(Rep.get_header(), DIALOG);
}

static	void	open_connection(t_server &server) {
	server_log("Openning connection...", DEBUG);

	if ((server.listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error_throw("open connection - socket creation - main.cpp");
	bzero (&server.servaddr, sizeof(server.servaddr));
	server.servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	server.servaddr.sin_port = htons(SERVER_PORT);

	if ((bind(server.listenfd, (SA *) &server.servaddr, sizeof(server.servaddr))) < 0)
		throw error_throw("Port " + int_to_str(SERVER_PORT));
	if (listen(server.listenfd, 10) < 0)
		throw error_throw("listen error - open_connection - main.cpp");
		
	server_log("Connection established on port " + int_to_str(SERVER_PORT), INFO);
	std::cout << std::string(GREENN) + "PORT " << SERVER_PORT << ": Ready for connection" + std::string(ENDD) + "\n";
	server_log("Initialization of poll variables\n", DEBUG);
	server.pfds.fd = server.listenfd;
	server.pfds.events = POLLIN;
}

static	void	init_log_file() {
	std::fstream log_file(LOG_FILE, std::ios::out | std::ios::trunc);
    
    if (!log_file.is_open())
		throw error_throw("log file cannot be created - init_log_file - main.cpp");
    log_file.close();
}

void	init_server(t_server &server) {
	g_sig = 0;
	signal(SIGINT, &sigint_handler);
	signal(SIGQUIT, &sigquit_handler);
	init_log_file();
	server_log("SERVER STARTED", INFO);
	open_connection(server);
}

void	stop_server(t_server &server) {
	close(server.listenfd);
	close(server.connfd);
	server_log("Connection closed", INFO);
	server_log("SERVER STOPPED", INFO);
	std::cout << std::string(YELLOWW) + "\nConnection closed\n" + std::string(ENDD);
}

int main()
{
	t_server	server;

	bzero(&server, sizeof(server));
	try {
		init_server(server);
		while (1) { /* Here is the main loop */
			if (poll(&server.pfds, 1, -1) == -1)
					error_throw("poll failure - main - main.cpp");
			if (server.pfds.revents & POLLIN \
			&& server.pfds.fd == server.listenfd) {
					memset(server.recvline, 0, MAXLINE);
					server.connfd = accept(server.listenfd, (SA *) NULL, NULL);
					send_response(server.connfd);
					close(server.connfd);
			}
		}
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << "\n";
		stop_server(server);
	}
}
