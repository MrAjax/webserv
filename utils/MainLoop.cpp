#include "MainLoop.hpp"

void    removeTimeout(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds)
{
	std::map<int, std::pair<struct sockaddr_in, HttpRequest* > >::iterator it = clientMap.begin();
	while (it != clientMap.end()) {
		if (it->second.second->checkTimeout()) {
			delete it->second.second;

			for (std::vector<struct pollfd>::iterator pollIt = pollfds.begin(); pollIt != pollfds.end(); ) {
				if (pollIt->fd == it->first) {
					close(pollIt->fd);
					pollIt = pollfds.erase(pollIt);
				} else {
					++pollIt;
				}
			}

			clientMap.erase(it++);
		} else
			++it;
	}
}

void    killRequest(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds, int i)
{
	clientMap[pollfds[i].fd].second->setMyserver(NULL);
	if (clientMap[pollfds[i].fd].second != NULL)
	{
    	delete clientMap[pollfds[i].fd].second;
		clientMap[pollfds[i].fd].second = NULL;
	}
	clientMap.erase(pollfds[i].fd);
	close(pollfds[i].fd);
	pollfds.erase(pollfds.begin() + i);
	// clientMap[pollfds[i].fd].second->resetRequest();
}

void    addingNewClient(HttpRequest **clientRequest, struct sockaddr_in &clientAddr, 
    std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds)
{
	clientMap[(*clientRequest)->getConnfd()] = std::make_pair(clientAddr, *clientRequest); //add client information to map Client
	struct pollfd newPfd;
	newPfd.fd = (*clientRequest)->getConnfd();
	newPfd.events = POLLIN;
	pollfds.push_back(newPfd); // add new fd to monitoring
	server_log("New connexion on fd " + int_to_str((*clientRequest)->getConnfd()) , DEBUG);
}