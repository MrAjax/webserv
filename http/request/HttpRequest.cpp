#include "HttpRequest.hpp"
#include "HttpRequestError.hpp"
#include "HttpRequestParsing.hpp"



#define MAXLINE 4096

HttpRequest::HttpRequest(void) : STATUS(NEW)
{
	
}

HttpRequest::HttpRequest(int connfd) : STATUS(NEW), _connfd(connfd)
{

}


HttpRequest::~HttpRequest(void){}

//-----------UTILS------------------

void		HttpRequest::printAttribute(void) //pour pouvoir print et vérifier que tout est bien parse
{
	struct _tab 
	{
		const char* key;                  // La clé
    	std::string (HttpRequest::*getter)();  // Pointeur vers le getter
	};
	std::size_t	nbParam = 16;
	struct _tab tab[nbParam] = {{"Method:", &HttpRequest::getMethod}, {"Path:", &HttpRequest::getPath},
		{"Protocol:", &HttpRequest::getHttp},{"Host:", &HttpRequest::getHost}, {"User-Agent:", &HttpRequest::getUserAgent},
		{"Accept:", &HttpRequest::getAccept}, {"Accept-Language:", &HttpRequest::getAcceptLanguage},
		{"Accept-Encoding:", &HttpRequest::getAcceptEncoding}, {"Connection:", &HttpRequest::getConnection},
		{"Upgrade-Insecure-Requests:", &HttpRequest::getUpInsecureRqst}, {"Referer:", &HttpRequest::getReferer},
		{"Sec-Fetch-Dest:", &HttpRequest::getSecFetchDest}, {"Sec-Fetch-Mode:", &HttpRequest::getSecFetchMode},
		{"Sec-Fetch-Site:", &HttpRequest::getSecFetchSite}, {"Content-Length:", &HttpRequest::getStrContentLength},
		{"Content-Type:", &HttpRequest::getContentType}};
	for (std::size_t i = 0; i < nbParam; i++)
		std::cout << YELLOW << tab[i].key << RESET << (this->*(tab[i].getter))() << std::endl;
}

//-----------Core functions----------

void	HttpRequest::recvfd(int & fd)
{
	u_int8_t recvline[MAXLINE + 1];
	memset(recvline, 0, MAXLINE);
	int numbytes;
	numbytes = recv(fd, recvline, MAXLINE - 1, 0);
	saveString += reinterpret_cast< char * >(recvline); 
	if (numbytes < 0)
		throw std::runtime_error("ERROR: Cannot read request\n");
	if (saveString.empty())
		throw std::runtime_error("ERROR: Request is empty\n");
}

int    HttpRequest::processingRequest(struct pollfd &request)
{
	try
	{
		recvfd(request.fd);
		if (STATUS < DONE_HEADER)
		{
			HttpRequestParsing header(*this);
			header.parsingHeader();
		}
		if (STATUS == DONE_HEADER)
		{
			HttpRequestError checkError(*this);
			checkError.Method();
		}
		if (STATUS != DONE_ALL && STATUS >= DONE_HEADER)
		{
			HttpRequestParsing body(*this);
			body.parsingBody();
		}
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	// std::cout << YELLOW  "STATUS after BODY " << STATUS << RESET << std::endl;
	printAttribute();
	return (STATUS);
}

// ---------------------GETTEUR---------------------------------

std::string HttpRequest::getMethod()			{return (this->_method);}
std::string HttpRequest::getPath()				{return (this->_path);}
std::string HttpRequest::getHttp()				{return (this->_http);}
std::string HttpRequest::getHost()				{return (this->_host);}
std::string HttpRequest::getUserAgent()			{return (this->_userAgent);}
std::string HttpRequest::getAccept()			{return (this->_accept);}
std::string HttpRequest::getAcceptLanguage()	{return (this->_acceptLanguage);}
std::string HttpRequest::getAcceptEncoding()	{return (this->_acceptEncoding);}
std::string HttpRequest::getConnection()		{return (this->_connection);}
std::string HttpRequest::getUpInsecureRqst()	{return (this->_upInsecureRqst);}
std::string HttpRequest::getReferer() 			{return (this->_referer);}
std::string HttpRequest::getSecFetchDest()		{return (this->_secFetchDest);}
std::string HttpRequest::getSecFetchMode()		{return (this->_secFetchMode);}
std::string HttpRequest::getSecFetchSite()		{return (this->_secFetchSite);}
std::size_t HttpRequest::getContentLength()		{return (this->_contentLength);}
std::string	HttpRequest::getContentType()		{return (this->_contentType);}

std::string HttpRequest::getBodyRequest()		{return (this->_bodyRequest);}
std::string HttpRequest::getHeaderRequest()		{return (this->_headerRequest);}

int			HttpRequest::getConnfd()			{return _connfd;}
std::string	HttpRequest::getInput()				{return _input;}
std::string HttpRequest::getSaveString()		{return (this->saveString);}

std::string HttpRequest::getStrContentLength()		{return (this->_strContentLength);}


//-------------------STETTEUR-------------------

void	HttpRequest::setMethod(const std::string &value)			{_method = value;}
void	HttpRequest::setPath(const std::string &value)				{_path = value;}
void	HttpRequest::setHttp(const std::string &value)				{_http = value;}
void	HttpRequest::setHost(const std::string &value)				{_host = value;}
void	HttpRequest::setUserAgent(const std::string &value)			{_userAgent = value;}
void	HttpRequest::setAccept(const std::string &value)			{_accept = value;}
void	HttpRequest::setAcceptLanguage(const std::string &value)	{_acceptLanguage = value;}
void	HttpRequest::setAcceptEncoding(const std::string &value)	{_acceptEncoding = value;}
void	HttpRequest::setConnection(const std::string &value)		{_connection = value;}
void	HttpRequest::setUpInsecureRqst(const std::string &value)	{_upInsecureRqst = value;}
void	HttpRequest::setReferer(const std::string &value) 			{_referer = value;}
void	HttpRequest::setSecFetchDest(const std::string &value)		{_secFetchDest = value;}
void	HttpRequest::setSecFetchMode(const std::string &value)		{_secFetchMode = value;}
void	HttpRequest::setSecFetchSite(const std::string &value)		{_secFetchSite = value;}
void	HttpRequest::setContentLength(const std::size_t &value)		{_contentLength = value;}
void	HttpRequest::setContentType(const std::string &value)		{_contentType = value;}

void	HttpRequest::setBodyRequest(const std::string &value)		{_bodyRequest = value;}
void	HttpRequest::setHeaderRequest(const std::string &value)		{_headerRequest = value;}

void	HttpRequest::setConnfd(const int &value)					{_connfd = value;}
void	HttpRequest::setInput(const std::string &value)				{_input = value;}
void	HttpRequest::setSaveString(const std::string &value)		{saveString = value;}

void	HttpRequest::setStrContentLength(const std::string &value)	{_strContentLength = value;}