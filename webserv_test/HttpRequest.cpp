#include "HttpRequest.hpp"

#define MAXLINE 4096

HttpRequest::HttpRequest(void){}

HttpRequest::~HttpRequest(void){}

void    HttpRequest::parsingHeader(int connfd)
{
    uint8_t recvline[MAXLINE + 1];
    memset(recvline, 0, MAXLINE);
    int n;

    while ((n = read(connfd, recvline, MAXLINE - 1)) > 0)
	{
		printf("%s\n", recvline);

        // if (recvline )
        // {

        // }


		if (recvline[n - 1] == '\n')
			break;
		memset(recvline, 0, MAXLINE);
	}
    if (n < 0)
        printf("read error\n");
}

