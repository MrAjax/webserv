#include "HttpRequest.hpp"

#define MAXLINE 4096

HttpRequest::HttpRequest(void){}

HttpRequest::~HttpRequest(void){}

void    HttpRequest::parsingHeader(int connfd)
{
    uint8_t recvline[MAXLINE + 1];
    memset(recvline, 0, MAXLINE);

    std::string fullRequest;
    int n;

    while ((n = read(connfd, recvline, MAXLINE - 1)) > 0)
	{
		printf("%s\n", recvline);

        // if (recvline )
        // {
                
        // }

        fullRequest += reinterpret_cast< char * >(recvline);
		if (recvline[n - 1] == '\n')
			break;
		memset(recvline, 0, MAXLINE);
	}
    if (n < 0)
        printf("read error\n");

    std::size_t pos = fullRequest.find("\r\n\r\n");
    if (pos == std::string::npos)
        std::cout << "NO HEADER END!!!!!!\n";

    std::string header;
    std::string data;
    std::string delimiteur = "\r\n\r\n";
    delimiteur = "\r\n\r\n";

    header = fullRequest.substr(0, pos);
    
    std::cout << "\nSIZE" << fullRequest.size() << " VS " << pos + delimiteur.size() << std::endl;
    
    std::cout << "\nHEADER\n[" + header + "]\n";
    if (fullRequest.size() > pos + delimiteur.size()) // s'il y a quelque chose après delimiteur alors c'est de la data à récup
    {
        data = fullRequest.substr(pos + delimiteur.size());
        std::cout << "\nDATA\n[" + data + "]\n";
    }

}

