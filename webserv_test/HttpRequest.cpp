#include "HttpRequest.hpp"

#define MAXLINE 4096

HttpRequest::HttpRequest(void){}

HttpRequest::~HttpRequest(void){}

std::string HttpRequest::getHeader(std::string &fullRequest)
{
	std::string delimiteur = "\r\n\r\n";
	std::size_t pos = fullRequest.find(delimiteur);
	if (pos == std::string::npos)
		std::cout << "NO HEADER END!!!!!!\n";
	std::string header = fullRequest.substr(0, pos);

	std::cout << "\nSIZE FULLREQUEST" << fullRequest.size() << " VS HEADER + DELIMITEUR " << pos + delimiteur.size() << std::endl; //test size
	std::cout << "\nHEADER\n[" + header + "]\n\n"; //print header

	return (header);
}
//-----------Header parser----------------

void	HttpRequest::parsingHeader_method_pathcmd_http(std::string &line)
{
	std::stringstream	ss(line);
	std::string			output;
	ss >> output;
	if (output != "GET" && output != "POST" && output != "DELETE")
		std::cout << "NO METHODE!!!!!!\n";
	else
		this->_method = output;
	ss >> output;
	this->_pathCmd;
	ss >> output;
	this->_pathCmd;
	ss >> output;
	this->_http;
}

void        HttpRequest::parsingHeader_host(std::string &line)
{
	std::stringstream	ss(line);
	std::string			output;
	ss >> output;
	if (output != "Host:")
		std::cout << "NO HOST!!!!!!\n";
	else
	{
		ss >> output;
		this->_host = output;
	}
}

void    HttpRequest::parsingHeader(std::string &header)
{
	std::string delimiteur = "\r\n";
	std::size_t pos = header.find(delimiteur);
	std::cout << "\n\n[" << header.substr(0, pos) << "]\n[\r\n]\n";

	std::string	line = header.substr(0, pos);
	parsingHeader_method_pathcmd_http(line);
}



void    HttpRequest::parsingHeader(int connfd)
{
	uint8_t recvline[MAXLINE + 1];
	memset(recvline, 0, MAXLINE);

	std::string fullRequest;
	int n;

	while ((n = read(connfd, recvline, MAXLINE - 1)) > 0)
	{
		printf("%s\n", recvline);
		fullRequest += reinterpret_cast< char * >(recvline);
		if (recvline[n - 1] == '\n')
			break;
		memset(recvline, 0, MAXLINE);
	}
	if (n < 0)
		printf("read error\n");

	getHeader(fullRequest);



}

