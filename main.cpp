/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 09:22:50 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/20 09:12:06 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp" 

int	main(int ac, char **av)
{
	if (ac != 2) {
		std::cerr << "Usage ./webserv config_file!" << std::endl;
		return 0;
	}

	std::vector<Server> servers;
	std::vector<struct pollfd> pollfds;
	std::map<int, Server*> serversMap;
	std::map<int, struct sockaddr_in> clientMap;

	if (readConfigFile(servers, av[1]) == -1)
		return 1;
	allocatePollFds(servers, pollfds); //set struct pollfd
	allocateServersMap(servers, serversMap); //set map pollfd.fd Server*

	for(;;) 
	{
		int poll_count = poll(&pollfds[0], pollfds.size(), -1);
		if (poll_count == -1) 
			std::cerr << "poll error: " << strerror(errno) << std::endl;

		for(size_t i = 0; i < pollfds.size(); i++)
		{
			if (pollfds[i].revents & POLLIN) //EVENT!
			{
				std::map<int, Server*>::iterator it = serversMap.find(pollfds[i].fd);
				if (it != serversMap.end()) //socketfd is listener
				{
					struct sockaddr_in clientAddr;
					socklen_t tempAddrlen = sizeof(clientAddr);
					int clientFd = accept(pollfds[i].fd, (struct sockaddr *)&clientAddr, &tempAddrlen); 
					if (clientFd == -1){
						std::cerr << "Accept error: " << strerror(errno) << std::endl;
					}
					else {
						clientMap[clientFd] = clientAddr; //add client information to map Client
						struct pollfd newPfd;
						newPfd.fd = clientFd;
						newPfd.events = POLLIN;
						pollfds.push_back(newPfd); // add new fd to monitoring
						std::cout << "New connection from " << clientAddr.sin_addr.s_addr << " on socket " << pollfds[i].fd;
						std::cout << " New fd " << clientFd << std::endl;
					}	
				}
				else // socketfd aldready set c/p from HttpRequest 
				{
					u_int8_t recvline[MAXLINE + 1];
					memset(recvline, 0, MAXLINE);
					int numbytes;
					numbytes = recv(pollfds[i].fd, recvline, MAXLINE - 1, 0);
					if (numbytes == 0) { //connection close
						std::map<int, struct sockaddr_in>::iterator it;
						it = clientMap.find(pollfds[i].fd);
						close(pollfds[i].fd);
						clientMap.erase(it); //delete fd client side
						pollfds.erase(pollfds.begin() + i); //delete fd monitoring side
					}	
				}
			}
		}
	}
	return 0;
}
