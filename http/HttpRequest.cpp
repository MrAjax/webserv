#include "HttpRequest.hpp"

#define MAXLINE 4096

HttpRequest::HttpRequest(void){}

HttpRequest::~HttpRequest(void){}

//-----------Guetteurs------------

std::string HttpRequest::getMethod() {return (this->_method);}
std::string HttpRequest::getPath() {return (this->_path);}
std::string HttpRequest::getHttp() {return (this->_http);}
std::string HttpRequest::getHost() {return (this->_host);}
std::string HttpRequest::getUserAgent() {return (this->_userAgent);}
std::string HttpRequest::getAccept() {return (this->_accept);}
std::string HttpRequest::getAcceptLanguage() {return (this->_acceptLanguage);}
std::string HttpRequest::getConnection() {return (this->_connection);}
std::string HttpRequest::getReferer() {return (this->_referer);}
std::string HttpRequest::getSecFetchDest() {return (this->_secFetchDest);}
std::string HttpRequest::getSecFetchMode() {return (this->_secFetchMode);}
std::string HttpRequest::getSecFetchSite() {return (this->_secFetchSite);}

//-----------Utils----------------

std::string	HttpRequest::findLine(std::string header, std::string &delimiteur, std::size_t & end_pos)
{
	std::string	line;
	end_pos = header.find(delimiteur);
	if (end_pos == std::string::npos)
		return (header);
	line = header.substr(0, end_pos);
	return (line);
}

//-----------Header parser----------------

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

void	HttpRequest::parsingHeader_method_path_http(std::string &line)
{
	std::stringstream	ss(line);
	std::string			output;
	ss >> output;
	if (output != "GET" && output != "POST" && output != "DELETE")
		std::cout << "ERROR NO METHODE!!!!!!\n";
	else
		this->_method = output;
	ss >> output;
	this->_path = output;
	ss >> output;
	this->_http = output;
	if (ss >> output)
		std::cout << "ERROR SHOULD BE EMPTY NOW!!!!!!\n";
}

std::string	HttpRequest::parsingHeader_rest(std::string &line, std::string const & keyWord)
{
	std::stringstream	ss(line);
	std::string			token;
	std::string			output;

	ss >> token;
	if (token != keyWord)
		std::cout << "ERROR NO " + keyWord + "!!!!!!\n";
	else
	{
		while (ss >> token)
		{
			if (!output.empty())
				output += " ";
			output += token;
		}
	}
	return (output);
}

void    HttpRequest::parseAllAttributes(std::string &header)
{
	std::string delimiteur = "\r\n";
	std::size_t start_pos = 0;
	std::size_t end_pos;
	std::string	line;
	std::string	tab_key[10] = {"Host:", "User-Agent:", "Accept:", "Accept-Language:", "Accept-Encoding:",
				"Connection:", "Referer:", "Sec-Fetch-Dest:", "Sec-Fetch-Mode:", "Sec-Fetch-Site:"};
	std::string *tab_ref[10] = {&_host, &_userAgent, &_accept, &_acceptLanguage, &_acceptEncoding,
				&_connection, &_referer, &_secFetchDest, &_secFetchMode, &_secFetchSite};

	line = findLine(header, delimiteur, end_pos);
	parsingHeader_method_path_http(line);
	//std::cout << "\n1[" << line << "]\n"; //print ligne pour voir ce que je parse
	for (int i = 0 ; i < 10; i++)
	{
		start_pos += end_pos + delimiteur.size();
		line = findLine(header.substr(start_pos), delimiteur, end_pos);
		*tab_ref[i] = parsingHeader_rest(line, tab_key[i]);
		//std::cout << "\n" << i << "[" << line << "]\n"; //print ligne pour voir ce que je parse
	}

	std::cout << "-------List command--------" << std::endl; //print pour voir si toutes les valeurs sont bien parse
	std::cout << this->_method << std::endl;
	std::cout << this->_path << std::endl;
	std::cout << this->_http << std::endl;
	for (int i = 0; i < 10; i++)
		std::cout << *tab_ref[i] << std::endl;
}

void    HttpRequest::parsingHeader(int connfd)
{
	u_int8_t recvline[MAXLINE + 1];
	memset(recvline, 0, MAXLINE);

	std::string fullRequest;
	int n;
	while ((n = read(connfd, recvline, MAXLINE - 1)) > 0)
	{
		std::cout << recvline << std::endl;
		fullRequest += reinterpret_cast< char * >(recvline);
		if (recvline[n - 1] == '\n')
			break;
		memset(recvline, 0, MAXLINE);
	}
	if (n < 0)
		std::cout << "read ERROR" << std::endl;

	std::string 	header = getHeader(fullRequest);
	parseAllAttributes(header);
}

