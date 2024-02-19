#include "request/HttpRequest.hpp"
#include "request/HttpRequestAllow.hpp"
#include "response/HttpResponse.hpp"
#include "inc/webserv.hpp"
#include "inc/parsing.hpp"
#include "utils/LoopUtils.hpp"
#include "clear/clear.hpp"

volatile int	g_sig;

int main(int ac, char **av)
{
	if (ac != 2) {
		std::cerr << "Usage ./webserv config_file!" << std::endl;
		return 0;
	}

		std::vector<Server>			servers;
		std::vector<struct pollfd>	pollfds;
		std::map<int, Server*>		serversMap;//http://localhost:18000/
		std::map<int, std::pair<struct sockaddr_in, HttpRequest* > > clientMap;
		std::string					dot[3] = {".  ", ".. ", "..."};
		int							n = 0;

		HttpRequestAllow check(50, 1000);
	
	try {
		init_server();
		readConfigFile(servers, av[1]);
		allocatePollFds(servers, pollfds); //set struct pollfd
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		for (size_t i = 0; i < servers.size(); i++) {
			if (servers[i].getSocketfd() != -1)
				close(servers[i].getSocketfd());
		}	
		return 1;
	}
	try {
		
		while (g_sig == 0) { /* Here is the main loop */
			std::size_t sizePollfds = pollfds.size();
		
			if (poll(&pollfds[0], sizePollfds, 1000) == -1)
				server_log("Poll error pollfds.size() = " + int_to_str(sizePollfds), ERROR); // TODO doit on renvoyer une erreur et fermer server?
			std::cout << "A\n";
			if (n == 3)
				n = 0;
			std::cout << "\rWebserv is running" << dot[n++] << std::flush;
			for(size_t i = 0; i < sizePollfds; i++)
			{
				if (pollfds[i].revents & POLLIN) //EVENT!
				{
					handlePollin(pollfds, clientMap, servers, check, i); 	
				}
				if (pollfds[i].revents & POLLOUT)
				{
					handlePollout(pollfds, clientMap, i); 	
				}
				if (pollfds[i].events & POLLERR)
				{
					handlePollerr(pollfds, clientMap, i);
				}
			}
			removeRequest(clientMap, pollfds, servers);
			removeTimeout(clientMap, pollfds);
		}
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << "\n";
	}
	exitClean(clientMap, pollfds);
}