#include "HttpStatusCode.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "../inc/webserv.hpp"

#define SERVER_PORT 18001

#define MAXLINE 4096
#define SA struct sockaddr

void	err_n_die(const char *fmt, ...) {
	(void)fmt;
	int		errno_save;
	va_list	ap;

	errno_save = errno;

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	fprintf(stdout, "\n");
	fflush(stdout);
	if (errno_save != 0) {
		std::cout << "(errno = " << errno_save << ") : " << strerror(errno_save) << "\n";
		std::cout << "\n";
		fflush(stdout);
	}
	va_end(ap);
	std::cout << "===> Error Exit and Ciao!\n";
	exit(1);
}

int main()
{
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	uint8_t				recvline[MAXLINE + 1];

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_n_die("Error while creating the socket!");

    bzero (&servaddr, sizeof(servaddr));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);

    if ((bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0)
		err_n_die("bind error.");
	if (listen(listenfd, 10) < 0)
		err_n_die("listen error.");

    
	struct pollfd pfds;
	pfds.fd = listenfd;
	pfds.events = POLLIN;
	
	for (;;)
	{
		std::cout << "PORT " << SERVER_PORT << ": Ready for connection\n";

		if (poll(&pfds, 1, -1) == -1)
			throw std::runtime_error("poll error\n");
		
		if (pfds.revents & POLLIN) {
			if (pfds.fd == listenfd) {
				connfd = accept(listenfd, (SA *) NULL, NULL); //NULL car on veut accepter n 'importe quelle connexion
				memset(recvline, 0, MAXLINE);
				HttpRequest Req;

				std::string		response;
				try {
					HttpResponse	Rep(Req);
					response = Rep.get_response();
				}
				catch (const std::exception &e) {
					std::cerr << "Error: " << e.what() << "\n";
				}
				std::cout << "=== Response ===\n" << response << "\n";
				write(connfd, response.c_str(), response.length());
				close(connfd);
			}
		}
		
    }
	close(listenfd);
}
