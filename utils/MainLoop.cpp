#include "MainLoop.hpp"

void    removeTimeout(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds)
{
    std::map<int, std::pair<struct sockaddr_in, HttpRequest* > >::iterator it = clientMap.begin();
	for (;it != clientMap.end(); it++)
	{
		if (it->second.second->checkTimeout())
		{
			delete it->second.second;
			for (std::size_t i = 0; i < pollfds.size() ; i++)
			{
				if (pollfds[i].fd == it->first)
				{
					close(pollfds[i].fd);
					pollfds.erase(pollfds.begin() + i);
				}
			}
			clientMap.erase(it->first);
		}
	}
}

void    killRequest(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds, int i)
{
    delete clientMap[pollfds[i].fd].second;
	clientMap.erase(pollfds[i].fd);
	close(pollfds[i].fd);
	pollfds.erase(pollfds.begin() + i);
}

void    addingNewClient(int &clientFd, struct sockaddr_in &clientAddr, 
    std::map<int, Server*> &serversMap, std::map<int, Server*>::iterator &it,
    std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds)
{
    HttpRequest *clientRequest = new HttpRequest(clientFd);
	serversMap[clientFd] = it->second;
	std::cout << it->second->getSocketfd() << std::endl;
	clientMap[clientFd] = std::make_pair(clientAddr, clientRequest); //add client information to map Client
	struct pollfd newPfd;
	newPfd.fd = clientFd;
	newPfd.events = POLLIN;
	pollfds.push_back(newPfd); // add new fd to monitoring
	server_log("New connexion on fd " + int_to_str(clientFd) , DEBUG);
}