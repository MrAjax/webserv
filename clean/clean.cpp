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
				server_log("ExitClean delete HttpRequest fail clientMap[" + int_to_str(it->first) + "]", ERROR);
			}
		}
	}
}

void removeTimeout(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds)
{
	std::map<int, std::pair<struct sockaddr_in, HttpRequest* > >::iterator it = clientMap.begin();
	for(; it != clientMap.end();)
	{
		int i = 0;
		bool matchPollfd = false;
		for (int index = pollfds.size(); index > 0; --index)
		{
			i = index - 1;
			if (it->first == pollfds[i].fd)
			{
				matchPollfd = true;
				break;
			}
		}
		if (matchPollfd == false)
			server_log("No match clientMap[val = " + int_to_str(it->first) + " ] and pollfds[x].fd = val", ERROR);
		if (it->second.second == NULL)
		{
			server_log("RemoveTimeout Error : clientMap[" + int_to_str(it->first) + "].request = NULL", ERROR);
			it++;
		}
		else if (matchPollfd && it->second.second->isKeepAliveTimeout())
		{
			it++;
			server_log("- Keep Alive Timeout -", DEBUG);
			killRequest(clientMap, pollfds, i);
		}
		else if (matchPollfd && it->second.second->isRequestTimeout())
		{
			it->second.second->setStatusCode(408);
			it++;
			server_log("- Request Timeout -", DEBUG);
			pollfds[i].revents = POLLOUT;
			// killRequest(clientMap, pollfds, i);
		}
		else
			it++;
	}
}

void	removeRequest(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds, std::vector<Server> servers)
{
	std::size_t index;
	for (std::size_t i = pollfds.size(); i > 0; --i)
    {
        index = i - 1;

        if (pollfds[index].fd == -1)
        {
            server_log("RemoveRequest pollfds[" + int_to_str(index) + "].fd = -1", ERROR);
            pollfds.erase(pollfds.begin() + index);
        }
        else if (isListener(pollfds[index].fd, servers) == false)
        {
            if (clientMap[pollfds[index].fd].second == NULL)
            {
                server_log("RemoveRequest Error : clientMap[pollfds[" + int_to_str(index)
                + "].fd].request = NULL, pollfds[" + int_to_str(index) + "] shouldn't exist", ERROR);
                close(pollfds[index].fd);
                pollfds.erase(pollfds.begin() + index);
            }
            else if (clientMap[pollfds[index].fd].second->getStatusCode() == KILL_ME)
			{
				server_log("- Remove Request -", DEBUG);
            	killRequest(clientMap, pollfds, index);
			}
        }
    }
}

bool    killRequest(std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > &clientMap, std::vector<struct pollfd> &pollfds, std::size_t i)
{
	if (pollfds[i].fd == -1)
	{
		server_log("KillRequest Error pollfds[" + int_to_str(i) + "].fd = -1", ERROR);
		return (false);
	}
	if (i < pollfds.size())
	{
		if (clientMap[pollfds[i].fd].second != NULL)
		{
			clientMap[pollfds[i].fd].second->setMyserver(NULL);
			try{
    			delete clientMap[pollfds[i].fd].second;
				clientMap[pollfds[i].fd].second = NULL;
			} catch(const std::exception& e) {
				server_log("Delete HttpRequest fail clientMap[pollfds[" + int_to_str(i) + "].fd] pollfds[" + int_to_str(i) + "].fd = ", ERROR);
			}
		}
		clientMap.erase(pollfds[i].fd);
		std::string strPollfd = int_to_str(pollfds[i].fd);
		close(pollfds[i].fd);
		pollfds.erase(pollfds.begin() + i);
		server_log("clientMap[pollfds[" + int_to_str(i) + "].fd] removed : pollfds[" + int_to_str(i) + "].fd = " + strPollfd, DEBUG);
		return (true);
	}
	server_log("KillRequest Error -> possible invalid read on clientMap " + int_to_str(i), ERROR);
	return (false);
}