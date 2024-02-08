#include "MainLoop.hpp"

void    removeTimeout(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds)
{
	std::map<int, std::pair<struct sockaddr_in, HttpRequest* > >::iterator it = clientMap.begin();
	while (it != clientMap.end())
	{
		if (it->second.second->checkTimeout()) {
			delete it->second.second;

			for (std::vector<struct pollfd>::iterator pollIt = pollfds.begin(); pollIt != pollfds.end(); )
			{
				if (pollIt->fd != -1 && pollIt->fd == it->first) {
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

void	removeRequest(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds, std::vector<Server> &servers)
{
	std::size_t i = 0;
	while (i < pollfds.size())
	{
		if (pollfds[i].fd == -1)
		{
			server_log(std::string(REDD) + "RemoveRequest pollfds[" + int_to_str(static_cast<std::size_t>(i)) + "].fd = -1", ERROR);
			pollfds.erase(pollfds.begin() + i);
		}
		else if (isListener(pollfds[i].fd, servers) == false)
		{
			if (clientMap[pollfds[i].fd].second == NULL)
			{
				server_log(std::string(REDD) + "RemoveRequest Error : clientMap[pollfds[" + int_to_str(static_cast<std::size_t>(i))
				+ "].fd].second = NULL, pollfds[x] shouldn't exist", ERROR);
				close(pollfds[i].fd);
				pollfds.erase(pollfds.begin() + i);
			}
			else if (clientMap[pollfds[i].fd].second->getStatusCode() >= 400 && killRequest(clientMap, pollfds, i) == false)
				i++;
		}
		else
			i++;
	}
}

bool    killRequest(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds, std::size_t i)
{
	if (i < pollfds.size())
	{
		if (clientMap[pollfds[i].fd].second != NULL)
		{
			clientMap[pollfds[i].fd].second->setMyserver(NULL);
    		delete clientMap[pollfds[i].fd].second;
			clientMap[pollfds[i].fd].second = NULL;
		}
		clientMap.erase(pollfds[i].fd);
		if (pollfds[i].fd != -1)
			close(pollfds[i].fd);
		else
			server_log("KillRequest pollfds[" + int_to_str(static_cast<std::size_t>(i)) + "].fd = -1", DEBUG);
		pollfds.erase(pollfds.begin() + i);
		return (true);
	}
	server_log(std::string(REDD) + "KillRequest Error -> possible invalid read on clientMap " + int_to_str(static_cast<std::size_t>(i)), ERROR);
	return (false);
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

bool isListener(int fd, std::vector<Server> &servers) {
	for (size_t i = 0; i < servers.size(); i++) {
		if (fd == servers[i].getSocketfd())
			return true;	
	}
	return false;
}
