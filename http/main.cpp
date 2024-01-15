#include "HttpStatusCode.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "../inc/webserv.hpp"

#define SERVER_PORT 18000

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

//pour debug permet de check s'il y a des char non printables qui posent pb
//prends une string de bytes et convertie en hex
char	*bin2hex(const unsigned char *input, size_t len)
{
	char	*result;
	char	hexits[] = "0123456789ABCDEF";

	if (input == NULL || len <= 0)
		return (NULL);

	int resultlength = (len * 3) + 1;

	result = (char*)malloc(resultlength);
	bzero(result, resultlength);

	for (size_t i = 0; i < len; i++) {
		result[i * 3] = hexits[input[i] >> 4];
		result[(i * 3) + 1] = hexits[input[i] & 0x0F];
		result[(i * 3) + 2] = ' ';
	}
	return (result);
}

int main()
{
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	uint8_t				recvline[MAXLINE + 1];

    // on essaie de creer le socket listenfd
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_n_die("Error while creating the socket!");

    //on accept une connexion ici on esai pas d eouter d ou la difference
    bzero (&servaddr, sizeof(servaddr)); //mise a zero de l adresse
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //je reponds a tout
	servaddr.sin_port = htons(SERVER_PORT); //On met pas le port 80 car on peut avoir besoin de le lancer en super user

    if ((bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0)
		err_n_die("bind error."); // on veut bind notre socket d ecoute a l adresse
	if (listen(listenfd, 10) < 0) // ensuite on ecoute
		err_n_die("listen error.");

    // en gros accepte la connection et la reset a l infini jusqu'a ce qu on en veuille plus
    
	// Poll set up below
	struct pollfd pfds;
	pfds.fd = listenfd;
	pfds.events = POLLIN;
	
	for (;;)
	{
		std::cout << "PORT " << SERVER_PORT << ": Ready for connection\n";
		//printf("waiting for a connection on port %d\n", SERVER_PORT);
		//fflush(stdout);
		//on attend une reponse avec accept, c est une commande bloquante
		// epoll ici
		if (poll(&pfds, 1, -1) == -1)
			throw std::runtime_error("poll error\n");
		
		if (pfds.revents & POLLIN) {
			if (pfds.fd == listenfd) {
				connfd = accept(listenfd, (SA *) NULL, NULL); //NULL car on veut accepter n 'importe quelle connexion
				memset(recvline, 0, MAXLINE);
				HttpRequest Head;
				HttpResponse Rep(Head);
				Rep.sendhtml(connfd, "text/html", "index.html");
				//Head.parsingHeader(connfd);
				/* std::string		response;
				try {
					HttpResponse	Rep(Head);
					response = Rep.get_response();
				}
				catch (...) {
					throw std::runtime_error("ERRRRRRROOOOOORRRR!!!!!!!!!!!!!!!!!!!!\n");
				}
				std::cout << "\n\n=== Response ===\n" << response << "\n\n"; */
				//write(connfd, response.c_str(), response.length());
				close(connfd);

/* 				//--------- test loading image ---------

				printf("waiting for a connection on port %d\n", SERVER_PORT);
				fflush(stdout);
				
				connfd = accept(listenfd, (SA *) NULL, NULL);
				memset(recvline, 0, MAXLINE);
			
				Head.parsingHeader(connfd);



				Rep.sendhtml(connfd, "image/jpeg", "landscape.jpeg");
				close(connfd); */
				break;
			}
		}
		
    }
	close(listenfd);
}
