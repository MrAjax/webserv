#include "request/HttpRequest.hpp"
#include "request/HttpRequestAllow.hpp"
#include "response/HttpResponse.hpp"
#include "inc/webserv.hpp"
#include "inc/parsing.hpp"
#include "utils/LoopUtils.hpp"
#include "clear/clear.hpp"

volatile int	g_sig;

/* 
	ERROR TO MANAGE IN THE REQUEST PARSER:

		error 405 - Method Not Allowed
	Si methode non connue -> error 405 - Method Not Allowed 

		408 - Request Timeout 
	Set chrono
	If timeout --> Send error 408 - Request Timeout 
	(Avoid the server to wait for an infinite time to process a request)

		411 - Length Required 
	If the field Content-Length is not set (for a post request i guess)
	then, send StatusCode 411 - Length Required

		413 - Payload Too Large
	If the request is too large to satisfy the server rules (very usefull for a post)
	then, send StatusCode 413 - Payload Too Large

		414 - URI Too Long

		415 - Unsuppported Media Type 
	(Use the Content Type Finder which returns an empty string in case of unsupported media type)
	
		429 - Too Many Requests
	Compter le nombre de requetes recues par client pour un labs de temps donne
	Definir une limite de requete par client sur un intervalle de temps a choisir
	Si trop de requests en meme temps (a la suite) pour un meme clent,
	alors send Status Code 429 - Too Many Requests

		431 - Request Header Fields Too Large 
	Verifier la taille du header, si header trop large --> Send 431 - Request Header Fields Too Large 

		501 - Not Implemented 
	Methode non supportee (PUT, HEAD, etc) --> Send 501 - Not Implemented 

		505 - HTTP Version Not Supported
	Si protocole different de HTTP/1.1 --> 505 - HTTP Version Not Supported
	
		Error 507 - Insufficient Storage
	Si pas assez de place pour stocker un POST --> Error 507 - Insufficient Storage

*/


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
