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
	// Send StatusCode 100 - Continue
	// Compter le nombre de requetes recues par client pour un labs de temps donne
	// Definir une limite de requete par client sur un intervalle de temps a choisir
	// Si trop de requests en meme temps (a la suite) pour un meme clent,
	// alors send Status Code 429 - Too Many Requests
	//
	// Set chrono
	HttpRequest 	Req(connfd);
	// If timeout --> Send error 408 - Request Timeout (Avoid the server to wait for an infinite time to process a request)
	server_log("Parsing Request...", DEBUG);
	std::string	request_header = Req.getHeaderRequest();
	// If the request is too large to satisfy the server rules (very usefull for a post)
	// then, send StatusCode 413 - Payload Too Large
	// If the field Content-Length is not set (for a post request i guess)
	// then, send StatusCode 411 - Length Required 
	// 414 - URI Too Long
	// 415 - Unsuppported Media Type (Use the Content Type Finder which 
	// returns an empty string in case of unsupported media type)
	// Verifierla taille du header, si header trop large --> Send 431 - Request Header Fields Too Large
	// 
	// Once all the chunks of the request are received --> Send StatusCode 102 - Processing
	if (request_header.empty()) {
		server_log("Unsupported request type", ERROR);
		// Renvoyer erreur appropriee et page erreur associee
		// Construire reponse erreur
		// Si methode non connue -> error 405 - Method Not Allowed 
		// Methode non supportee (PUT, HEAD, etc) --> Send 501 - Not Implemented 
		// Si protocole different de HTTP/1.1 --> 505 - HTTP Version Not Supported
		// Si pas assez de place pour stocker un POST --> Error 507 - Insufficient Storage
		// 
		// Requete invalide --> Send StatusCode 400 - Bad Request et envoyer page associee
		// continuer (supprimer le return)
		// (Error 50x -> the server should not crash or stop unless stopped manually so if
		// a system call fails or something else while inside of the main loop, send error 50x)
		throw std::runtime_error(std::string(REDD) + "Unsupported request type" + std::string(ENDD));
	}
	server_log("Request is valid", DEBUG);
	// Send StatusCode 202 - Accepted
	server_log(request_header + "\n\n", DIALOG);
	server_log("Building Response...", DEBUG);
	HttpResponse	Rep(Req);
	// Check for the rights of the request	
	// Not the right rights ? --> Send Status code 403 + associated page
	// Check if the user is logged (user or admin)
	// If the request wants to access a welcome page for example and there is no authentication
	// then, send Status code 401 - Unauthorized
	std::string		response(Rep.get_response());
	// If there is no requested ressource is not found --> error 404 - Not Found
	server_log("Sending response...", DEBUG);
	write(connfd, response.c_str(), response.length());
	server_log("Response sent", INFO);
	server_log(Rep.get_header(), DIALOG);
	// Remove the client in case of an error
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
	std::cout << std::string(GREENN) + "\nConnection closed - Bye!\n" + std::string(ENDD);
}

int main()
{
	t_server	server;

	bzero(&server, sizeof(server));
	try {
		init_server(server);
		while (g_sig == 0) { /* Here is the main loop */
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
