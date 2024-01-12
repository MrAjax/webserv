#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>

#define SERVER_PORT 18000
#define MAXLINE 4096
#define SA struct sockaddr

void	err_n_die(const char *fmt, ...) {
	int		errno_save;
	va_list	ap;

	errno_save = errno;

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	fprintf(stdout, "\n");
	fflush(stdout);

	if (errno_save != 0) {
		fprintf(stdout, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
		fprintf(stdout, "\n");
		fflush(stdout);
	}
	va_end(ap);
	exit(1);
}

int main(int argc, char **argv)
{
    int					sockfd, n;
	int					sendbytes;
	struct sockaddr_in	servaddr;
	char				sendline[MAXLINE];
	char				recvline[MAXLINE];

    //Usage check
    if (argc != 2)
		err_n_die("usage: %s <server address>", argv[0]);

    //Create socket
    //AF_INET = adresse family internet = internet socket
    //SOCK_STREAM = stream socket (il existe aussi les datagram socket)
    //0 = protocol par default (TCP)
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_n_die("Error while creating the socket!");

    bzero (&servaddr, sizeof(servaddr)); //mise a zero de l adresse
	servaddr.sin_family = AF_INET; //specification du type d'adresse (adresse internet ici)
	servaddr.sin_port = htons(SERVER_PORT); //specification du port htons()= host to network, short ca permet de rendre compatible entre ordi 


    //transform l'adresse sous forme de string en binaire
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_n_die("inet_pton error for %s", argv[1]);

    //on se connect au server
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_n_die("connect failed!");

    //si on arrive a se connecter on envoie la commande GET pour la page root "/" sous http1.1 "GET / HTTP/1.1\r\n\r\n"
    sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
	sendbytes = strlen(sendline);

    //j ecris tout dans le socket qui va l'envoyer au server
    if (write(sockfd, sendline, sendbytes) != sendbytes)
		err_n_die("write error");


    memset(recvline, 0, MAXLINE);
    //on lit la reponse du server
	while ((n = read(sockfd, recvline, MAXLINE - 1)) > 0) {
		printf("%s", recvline);
		memset(recvline, 0, MAXLINE); //on reset a 0 recvline a chaque fois
	}
	if (n <  0)
		err_n_die("read error");
    exit (0);
}