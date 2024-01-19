#include "HttpRequest.hpp"
#include "HttpStatusCode.hpp"

#define MAXLINE 4096

HttpRequest::HttpRequest(int connfd)
{
	try{
	parsingHeader(connfd);
	checkError();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

HttpRequest::HttpRequest(int connfd, std::string contentType, std::string input) {
	_connfd = connfd;
	_contentType = contentType;
	_input = input;
}

HttpRequest::~HttpRequest(void){}

//-----------Check Errors------------

void	HttpRequest::checkError(void)
{
	std::string *key_infos[4] = {&_method , &_path, &_http, &_host};

	for (int i = 0; i < 4; i++)
		if ((*key_infos[i]).empty())
			throw std::runtime_error("ERROR: Key info missing in request\n");
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		throw std::runtime_error("ERROR: Dont now the method in request\n");
	if (_http != "HTTP/1.1")
		throw std::runtime_error("ERROR: Bad protocol in request\n");
	if (_path != "/")
		if (access("index.html", F_OK) != 0)
			throw std::runtime_error("ERROR: Cant acces file\n");
}

//-----------Guetteurs------------

std::string HttpRequest::getMethod()			{return (this->_method);}
std::string HttpRequest::getPath()				{return (this->_path);}
std::string HttpRequest::getHttp()				{return (this->_http);}
std::string HttpRequest::getHost()				{return (this->_host);}
std::string HttpRequest::getUserAgent()			{return (this->_userAgent);}
std::string HttpRequest::getAccept()			{return (this->_accept);}
std::string HttpRequest::getAcceptLanguage()	{return (this->_acceptLanguage);}
std::string HttpRequest::getConnection()		{return (this->_connection);}
std::string HttpRequest::getUpInsecureRqst()	{return (this->_upInsecureRqst);}
std::string HttpRequest::getReferer() 			{return (this->_referer);}
std::string HttpRequest::getSecFetchDest()		{return (this->_secFetchDest);}
std::string HttpRequest::getSecFetchMode()		{return (this->_secFetchMode);}
std::string HttpRequest::getSecFetchSite()		{return (this->_secFetchSite);}

std::string HttpRequest::getBodyRequest()		{return (this->_bodyRequest);}
std::string HttpRequest::getHeaderRequest()		{return (this->_headerRequest);}

int			HttpRequest::getConnfd()			{return _connfd;}
std::string	HttpRequest::getContentType()		{return _contentType;}
std::string	HttpRequest::getInput()				{return _input;}

//-----------Utils----------------

std::size_t	HttpRequest::findLine(std::string &header, std::string &line, std::string &delimiteur)
{
	std::size_t	end_pos = header.find(delimiteur);
	if (end_pos == std::string::npos)
	{
		line = header;
		return (end_pos);
	}
	line = header.substr(0, end_pos);
	header = header.substr(end_pos + delimiteur.size());
	return (end_pos);
}

//-----------Header parser----------------

void	HttpRequest::splitHeaderBody(std::string &fullRequest)
{
	std::string delimiteur = "\r\n\r\n";
	std::size_t pos = fullRequest.find(delimiteur);
	if (pos == std::string::npos)
		throw std::runtime_error("ERROR: Cannot find end of header in request\n");
	_headerRequest = fullRequest.substr(0, pos);
	if (fullRequest.size() > pos + delimiteur.size())
		_bodyRequest = fullRequest.substr(pos + delimiteur.size());
}

void	HttpRequest::parsingHeader_method_path_http(std::string &line)
{
	std::stringstream	ss(line);
	std::string			output;
	ss >> output;
	if (output != "GET" && output != "POST" && output != "DELETE")
		throw std::runtime_error("ERROR: Cannot find method in request\n");
	else
		this->_method = output;
	ss >> output;
	this->_path = output;
	ss >> output;
	this->_http = output;
	if (ss >> output)
		throw std::runtime_error("ERROR: Bad request synthax in 1st line\n");
}

std::string	HttpRequest::parsingHeader_rest(std::string &line, std::string const & keyWord)
{
	std::stringstream	ss(line);
	std::string			token;
	std::string			output;

	ss >> token;
	if (token != keyWord)
		return (output);
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

void    HttpRequest::parseAllAttributes(std::string header)
{
	std::string delimiteur = "\r\n";
	std::size_t end_pos;
	std::string	line;
	std::string	tab_key[11] = {"Host:", "User-Agent:", "Accept:", "Accept-Language:", "Accept-Encoding:",
				"Connection:", "Upgrade-Insecure-Requests:", "Referer:", "Sec-Fetch-Dest:", "Sec-Fetch-Mode:", "Sec-Fetch-Site:"};
	std::string *tab_ref[11] = {&_host, &_userAgent, &_accept, &_acceptLanguage, &_acceptEncoding,
				&_connection, &_upInsecureRqst, &_referer, &_secFetchDest, &_secFetchMode, &_secFetchSite};

	end_pos = findLine(header, line, delimiteur);
	parsingHeader_method_path_http(line);
	while (end_pos != std::string::npos)
	{
		end_pos = findLine(header, line, delimiteur);
		for (int i = 0 ; i < 11; i++)
		{
			if (line.find(tab_key[i]) != std::string::npos)
			{
				if (!(*tab_ref[i]).empty())
					throw std::runtime_error("Error: Bad request synthax");
				*tab_ref[i] = parsingHeader_rest(line, tab_key[i]);
				break ;
			}
		}
	}

	std::cout << "-------List command--------" << std::endl; //print pour voir si toutes les valeurs sont bien parse
	std::cout << this->_method << std::endl;
	std::cout << this->_path << std::endl;
	std::cout << this->_http << std::endl;
	for (int i = 0; i < 11; i++)
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
		// std::cout << recvline << std::endl;
		fullRequest += reinterpret_cast< char * >(recvline);
		if (recvline[n - 1] == '\n')
			break;
		memset(recvline, 0, MAXLINE);
	}
	if (n < 0)
		throw std::runtime_error("ERROR: Cannot read request\n");
	if (fullRequest.empty())
		throw std::runtime_error("ERROR: Request is empty\n");

	splitHeaderBody(fullRequest);
	parseAllAttributes(_headerRequest);

}
