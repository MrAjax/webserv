#include "clean.hpp"

void	exitClean(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds)
{
	for (size_t i = 0; i < pollfds.size(); i++)
	{
		if (pollfds[i].fd == -1)
			server_log(std::string(REDD) + "ExitClean pollfds[" + int_to_str(i) + "].fd = -1", ERROR);
		else
			close(pollfds[i].fd);
	}
	for (std::map<int, std::pair<struct sockaddr_in, HttpRequest* > >::iterator it = clientMap.begin();
		it != clientMap.end(); it++)
	{
		if (it->second.second != NULL)
		{
			try
			{
				delete it->second.second;
				it->second.second = NULL;
			}
			catch(const std::exception& e)
			{
				server_log(std::string(REDD) + "ExitClean delete HttpRequest fail clientMap[" + int_to_str(it->first) + "]", ERROR);
			}
		}
	}
}

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

void	removeRequest(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds, std::vector<Server> servers)
{
	for (std::size_t i = pollfds.size(); i > 0; --i)
    {
        std::size_t index = i - 1;
        if (pollfds[index].fd == -1)
        {
            server_log(std::string(REDD) + "RemoveRequest pollfds[" + int_to_str(index) + "].fd = -1", ERROR);
            pollfds.erase(pollfds.begin() + index);
        }
        else if (isListener(pollfds[index].fd, servers) == false)
        {
            if (clientMap[pollfds[index].fd].second == NULL)
            {
                server_log(std::string(REDD) + "RemoveRequest Error : clientMap[pollfds[" + int_to_str(index)
                + "].fd].second = NULL, pollfds[x] shouldn't exist", ERROR);
                close(pollfds[index].fd);
                pollfds.erase(pollfds.begin() + index);
            }
            else if (clientMap[pollfds[index].fd].second->getStatusCode() == KILL_ME)
            	killRequest(clientMap, pollfds, index);
        }
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
			server_log("KillRequest pollfds[" + int_to_str(i) + "].fd = -1", DEBUG);
		pollfds.erase(pollfds.begin() + i);
		server_log("clientMap[pollfds[" + int_to_str(i) + "].fd] removed : pollfds[i].fd = " + int_to_str(pollfds[i].fd), DEBUG);
		return (true);
	}
	server_log(std::string(REDD) + "KillRequest Error -> possible invalid read on clientMap " + int_to_str(i), ERROR);
	return (false);
}