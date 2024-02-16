#include "../request/HttpRequest.hpp"
#include "../request/HttpRequestAllow.hpp"
#include "../response/HttpResponse.hpp"
#include "../response/ResponseSender.hpp"

#include "../inc/webserv.hpp"
#include "../inc/parsing.hpp"
#include "../utils/LoopUtils.hpp"
#include "../clear/clear.hpp"

static void handleFirstConnection(std::vector< struct pollfd> & pollfds, std::map<int, std::pair<struct sockaddr_in, HttpRequest*> > & clientMap, std::vector<Server> & servers, HttpRequestAllow & check, size_t i)
{
	server_log("--------- SocketFd " + int_to_str(pollfds[i].fd) + " accept new client ---------", DIALOG);

	struct sockaddr_in clientAddr;
	socklen_t tempAddrlen = sizeof(clientAddr);
	int clientFd = accept(pollfds[i].fd, (struct sockaddr *)&clientAddr, &tempAddrlen);
	if (clientFd == -1) {
		server_log("Accept error for socketfd " + int_to_str(pollfds[i].fd), ERROR);
	}
	else {
		try
		{
			HttpRequest *clientRequest = new HttpRequest(clientFd, servers, pollfds[i].fd);
			check.allowRequest(pollfds, *clientRequest);
			addingNewClient(&clientRequest, clientAddr, clientMap, pollfds);
		}
		catch (const std::exception &e)
		{
			server_log("HttpRequest allocation fail clientfd " + int_to_str(clientFd), ERROR);
		}
	}
}		

void handlePollin(std::vector< struct pollfd> & pollfds, std::map<int, std::pair<struct sockaddr_in, HttpRequest*> > & clientMap, std::vector<Server> & servers, HttpRequestAllow & check, size_t i)
{
	if (isListener(pollfds[i].fd, servers)) //socketfd is listener == 1st co
			handleFirstConnection(pollfds, clientMap, servers, check, i);
	else // socketfd aldready set c/p from HttpRequest
	{
		server_log("--------- Processing request clientFd " + int_to_str(pollfds[i].fd) + " ---------", DIALOG);
		if (clientMap[pollfds[i].fd].second->processingRequest() >= 200)
		{
			pollfds[i].events = POLLOUT;
			pollfds[i].revents = 0;
		}
	}
}

void handlePollout(std::vector< struct pollfd> & pollfds, std::map<int, std::pair<struct sockaddr_in, HttpRequest*> > & clientMap, size_t i)
{
	server_log("--------- Sending response to clientFd " + int_to_str(pollfds[i].fd) + " ---------", DIALOG);
	if (clientMap[pollfds[i].fd].second->getStatusCode() != KILL_ME)
		ResponseSender SendingResponse(*clientMap[pollfds[i].fd].second, pollfds[i], clientMap[pollfds[i].fd].second->getMyserver());
		// send_response(pollfds[i].fd, *clientMap[pollfds[i].fd].second->getMyserver(), *clientMap[pollfds[i].fd].second);
	// if (clientMap[pollfds[i].fd].second->getStatusCode() == KILL_ME)
	// 	server_log("Set clientFd " + int_to_str(pollfds[i].fd) + " to close", DEBUG);
	// else
	// 	{
	// 		server_log("Reset clientFd " + int_to_str(pollfds[i].fd) + " for other requests (keep-alive)", DEBUG);
	// 		pollfds[i].events = POLLIN;
	// 		pollfds[i].revents = 0;
	// 		clientMap[pollfds[i].fd].second->resetRequest();
	// 	}
}

void handlePollerr(std::vector< struct pollfd> & pollfds, std::map<int, std::pair<struct sockaddr_in, HttpRequest*> > & clientMap, size_t i)
{
	server_log("--------- Timeout clientFd " + int_to_str(pollfds[i].fd) + " ---------", DIALOG);
	send_response(pollfds[i].fd, *clientMap[pollfds[i].fd].second->getMyserver(), *clientMap[pollfds[i].fd].second);
	if (fcntl(pollfds[i].fd, F_SETFL, fcntl(pollfds[i].fd, F_GETFL) & ~O_NONBLOCK) == -1) {
		server_log("fcntl pollfds[" + int_to_str(i) + "].fd failed to set back blocking behavior", ERROR);
	}
}	
