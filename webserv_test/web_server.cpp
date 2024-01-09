#include "web_server.hpp"
#include "HttpRequest.hpp"
#include <string.h>
#include <sstream>
#include <cstring>
#include <iostream>

void sendhtml(int connfd)
{
	std::ifstream file("index.html");

	if (file.is_open())
	{
		std::stringstream ss;
		ss << file.rdbuf();
		file.close();
	
	std::string htmlcontent = ss.str();

	std::string response;
	response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + std::to_string(htmlcontent.length()) + "\r\n\r\n";
    response += htmlcontent;

	write(connfd, response.c_str(), response.length());
	}
	else{
		std::cerr << "Error HTML" << std::endl;
	}

}

int main(int argc, char **argv)
{
	int					listenfd, connfd, n;
	struct sockaddr_in	servaddr;
	uint8_t				buff[MAXLINE + 1];
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
    for (;;)
	{
		struct sockaddr_in	addr;
		socklen_t			addr_len;

		printf("waiting for a connection on port %d\n", SERVER_PORT);
		fflush(stdout);
		//on attend une reponse avec accept, c est une commande bloquante
		connfd = accept(listenfd, (SA *) NULL, NULL); //NULL car on veut accepter n 'importe quelle connexion
		memset(recvline, 0, MAXLINE);

        //on read un nb de byte et on print --- une version hexa est mise a cote pour check les char non printables bin2hex()
		
		HttpRequest Head;
		Head.parsingHeader(connfd);

        // On envoie une reponse en http1.0 200 OK
        // snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nHello");
        // write(connfd, (char *)buff, strlen((char *)buff));
		printf("connfd->[%d]\n", connfd);
		sendhtml(connfd);
		close(connfd);
        
    }
}