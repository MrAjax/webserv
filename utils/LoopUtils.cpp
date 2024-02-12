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

void    addingNewClient(HttpRequest **clientRequest, struct sockaddr_in &clientAddr, 
    std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds)
{
	clientMap[(*clientRequest)->getConnfd()] = std::make_pair(clientAddr, *clientRequest); //add client information to map Client
	struct pollfd newPfd;
	newPfd.fd = (*clientRequest)->getConnfd();
	if ((*clientRequest)->getStatusCode() != NEW)
		newPfd.events = POLLOUT;
	else
		newPfd.events = POLLIN;
	pollfds.push_back(newPfd); // add new fd to monitoring
	server_log("New connexion on fd " + int_to_str((*clientRequest)->getConnfd()) , DEBUG);
}

//---------- SEND RESPONSe ---------

void	send_response_to_client(int connfd, std::string response) {
	server_log("Sending response...", DEBUG);
	int numbytes;
	numbytes = send(connfd, response.c_str(), response.length(), 0);
	if (numbytes == -1)
		server_log("Client fd " + int_to_str(connfd) + " send failed client has closed connexion", ERROR);
	else if (numbytes == 0)
		server_log("Client fd " + int_to_str(connfd) + " no bytes send", ERROR);
	else
		server_log("Response sent bytes numbers = " + int_to_str(numbytes), INFO);
}

void	send_response(int connfd, Server &serv ,HttpRequest &Req) {
	std::string	response;
	server_log("Sending response to clientFd " + int_to_str(connfd) + "...", DEBUG);
	try {
		if (connfd < 0)
		{
			server_log("pollfds[i].fd = -1 HttpRequest fd = " + int_to_str(Req.getConnfd()), ERROR);
			throw	StatusSender::send_status(500, serv, false);
		}
		if (Req.getMyserver() == NULL && Req.getStatusCode() >= 400)
			throw	StatusSender::send_status(Req.getStatusCode(), serv, false);
		else if (Req.getStatusCode() >= 400)
			throw	StatusSender::send_status(Req.getStatusCode(), serv, true);
		else if (Req.getMyserver() == NULL)
		{
			server_log("No server in request clientFd " + int_to_str(connfd) + " status code = " + int_to_str(Req.getStatusCode()), ERROR);
			throw	StatusSender::send_status(500, serv, false);
		}
		if (Req.getHeaderRequest().empty()) {
			server_log("ClientFd " + int_to_str(connfd) + " invalid request : no header", ERROR);
			server_log("ClientFd " + int_to_str(connfd) + " request status code : " + int_to_str(Req.getStatusCode()), ERROR);
			throw	StatusSender::send_status(400, serv, true);
		}
		server_log("ClientFd " + int_to_str(connfd) + " server name is " + serv.getServerName(), DEBUG);
		server_log("Request is valid", DEBUG);
		server_log(Req.getHeaderRequest() + "\n\n", DIALOG);
		server_log(Req.getBodyRequest() + "\n\n", DIALOG);
		server_log("Building Response...", DEBUG);

		HttpResponse	Rep(Req, serv);

		response = Rep.get_response(serv);
		server_log(Rep.get_header(), DIALOG);
		send_response_to_client(connfd, response);
		if (Req.getConnection() == "close" || Req.getStatusCode() >= 400)
			Req.setStatusCode(KILL_ME);
	}
	catch (std::string &s) {
		response = s;
		server_log(response, DIALOG);
		send_response_to_client(connfd, response);
		Req.setStatusCode(KILL_ME);
	}
}