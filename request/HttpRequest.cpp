#include "HttpRequest.hpp"

#define MAXLINE 4096

HttpRequest::HttpRequest(void)
{
	STATUS = NEW;
	fullRequest = "";
}

HttpRequest::HttpRequest(int connfd)
{
	_connfd = connfd;
	STATUS = NEW;
	fullRequest = "";
}


HttpRequest::~HttpRequest(void){}

//-----------Core functions----------

void	HttpRequest::recvfd(int & fd)
{
	u_int8_t recvline[MAXLINE + 1];
	memset(recvline, 0, MAXLINE);
	int numbytes;
	numbytes = recv(fd, recvline, MAXLINE - 1, 0);
	fullRequest += reinterpret_cast< char * >(recvline); 
	if (numbytes < 0)
		throw std::runtime_error("ERROR: Cannot read request\n");
	if (fullRequest.empty())
		throw std::runtime_error("ERROR: Request is empty\n");
}

int    HttpRequest::processingRequest(struct pollfd &request)
{
	try
	{
		recvfd(request.fd);
		if (STATUS < DONE_HEADER)
			parsingHeader();
		if (STATUS != DONE_ALL && STATUS >= DONE_HEADER)
			parsingBody();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	std::cout << YELLOW  "STATUS after BODY " << STATUS << RESET << std::endl;
	return (STATUS);
}

void    HttpRequest::parsingHeader(void)
{
	if (STATUS >= DONE_HEADER)
		return ;
	std::string delimiteur = "\r\n\r\n";
	std::size_t pos = fullRequest.find(delimiteur);
	if (pos == std::string::npos)
	{
		STATUS = PROCESSING_HEADER;
		return ;
	}
	_headerRequest = fullRequest.substr(0, pos);
	if (fullRequest.size() > pos + delimiteur.size())
		fullRequest = fullRequest.substr(pos + delimiteur.size());
	else
		fullRequest = "";
	parseAllAttributes(_headerRequest);
}

void    HttpRequest::parsingBody(void)
{
	if (STATUS == DONE_ALL || STATUS < DONE_HEADER)
		return ;
	
	if (_contentLength.empty())
	{
		STATUS = DONE_ALL;
		return ;
	}
	std::size_t	contlength =  convert(_contentLength);
	if (fullRequest.size() >= contlength)
	{
		_bodyRequest = fullRequest.substr(0, contlength);
		fullRequest = "";
		STATUS = DONE_ALL;
	}
	else
		STATUS = PROCESSING_BODY;
}

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

std::size_t HttpRequest::convert(std::string &toConvert)
{
	if (toConvert.empty())
		throw std::runtime_error("ERROR: string to size_t convertor nothing to convert\n");
	if (toConvert[0] == '0' && toConvert.size() > 1)
		throw std::runtime_error("ERROR: string to size_t convertor bad synthax\n");
	for (std::size_t i = 0; i < toConvert.size(); i++)
	{
		if (!std::isdigit(toConvert[i]))
			throw std::runtime_error("ERROR: string to size_t convertor != digit\n");
	}
	std::stringstream ss;
	ss << toConvert;
	std::size_t	value;
	ss >> value;
	if (value == std::numeric_limits<std::size_t>::max())
		throw std::runtime_error("ERROR: string to size_t convertor value >= size_t MAX\n");
	return (value);
}

//-----------Header parser----------------

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
	std::size_t nbParam = 13;
	std::size_t end_pos;
	std::string delimiteur = "\r\n";
	std::string	line;
	std::string	tab_key[nbParam] = {"Host:", "User-Agent:", "Accept:", "Accept-Language:", "Accept-Encoding:",
				"Connection:", "Upgrade-Insecure-Requests:", "Referer:", "Sec-Fetch-Dest:", "Sec-Fetch-Mode:",
				"Sec-Fetch-Site:", "Content-Length:", "Content-Type:"};
	std::string *tab_ref[nbParam] = {&_host, &_userAgent, &_accept, &_acceptLanguage, &_acceptEncoding,
				&_connection, &_upInsecureRqst, &_referer, &_secFetchDest, &_secFetchMode,
				&_secFetchSite, &_contentLength, &_contentType};

	end_pos = findLine(header, line, delimiteur);
	parsingHeader_method_path_http(line);
	while (end_pos != std::string::npos)
	{
		end_pos = findLine(header, line, delimiteur);
		for (std::size_t i = 0 ; i < nbParam; i++)
		{
			if (line.find(tab_key[i]) != std::string::npos)
			{
				if (!(*tab_ref[i]).empty())
					throw std::runtime_error("Error: Bad request synthax duplicate information");
				*tab_ref[i] = parsingHeader_rest(line, tab_key[i]);
				break ;
			}
		}
	}
	STATUS = DONE_HEADER;
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
std::string HttpRequest::getContentLength()		{return (this->_contentLength);}
std::string	HttpRequest::getContentType()		{return (this->_contentType);}

std::string HttpRequest::getBodyRequest()		{return (this->_bodyRequest);}
std::string HttpRequest::getHeaderRequest()		{return (this->_headerRequest);}

int			HttpRequest::getConnfd()			{return _connfd;}
std::string	HttpRequest::getInput()				{return _input;}