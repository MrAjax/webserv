#include "HttpRequest.hpp"
#include "HttpRequestChecking.hpp"
#include "HttpRequestParsing.hpp"

//throw StatusSender::send_status(500, serv, true); quand le server et la requete n'est pas conforme il faut envoyer ca et pas un autre truc avec erreur 500 le server ne doit pas s arreter


HttpRequest::HttpRequest(int connfd, std::vector<Server> &servers, int listenFd) : _method(""), _path(""), _http(""),
_host(""), _userAgent(""), _accept(""), _acceptLanguage(""), _acceptEncoding(""),
_connection(""), _upInsecureRqst(""), _referer(""), _secFetchDest(""), _secFetchMode(""),
_secFetchSite(""), _contentLength(0), _contentType(""), _cookie(""),
_bodyRequest(""), _headerRequest(""),
_connfd(connfd), saveString(""), _strContentLength(""),
_servers(servers), _myServer(NULL),
_statusCode(NEW), _isCgi(false), _listenFd(listenFd), _maxBodySize(0)
{
	clock_gettime(CLOCK_REALTIME, &_keepAliveTimeout);
	clock_gettime(CLOCK_REALTIME, &_requestTimeout);
	std::stringstream ss;
	ss << _connfd;
	_debugFd = ss.str();
	// std::cout << BLUE << _connfd << " Constructor call\n" << RESET;
}

HttpRequest::~HttpRequest(void)	{/*std::cout << BLUE << _connfd << " Destructor call\n" << RESET;*/}

//-----------UTILS------------------

bool	HttpRequest::isKeepAliveTimeout(void) // permet de check si on doit kill la requette car timout
{
	struct timespec end;
	clock_gettime(CLOCK_REALTIME, &end);
	int timeSec = end.tv_sec - _keepAliveTimeout.tv_sec;
	if (timeSec > KEEP_ALIVE_TIMEOUT)
		return (true);
	else
		return (false);
}

bool	HttpRequest::isRequestTimeout(void) 
{
	if (_statusCode == NEW_BORN)
	{
		clock_gettime(CLOCK_REALTIME, &_requestTimeout);
		return (false);
	}
	struct timespec end;
	clock_gettime(CLOCK_REALTIME, &end);
	int timeSec = end.tv_sec - _requestTimeout.tv_sec;
	if (timeSec > REQUEST_TIMEOUT)
		return (true);
	else
		return (false);
}

void		HttpRequest::printAttribute(void) //pour pouvoir print et vérifier que tout est bien parse
{
	struct _tab 
	{
		const char* key;                  // La clé
    	std::string (HttpRequest::*getter)();  // Pointeur vers le getter
	};
	std::cout << YELLOW << "-----MY FD IS " << _connfd << " -----"<< RESET << std::endl;
	std::cout << YELLOW << "status code: " << RESET << _statusCode << std::endl;

 	struct _tab tab[NBPARAM] = {{"Method:", &HttpRequest::getMethod}, {"Path:", &HttpRequest::getPath},
		{"Protocol:", &HttpRequest::getHttp},{"Host:", &HttpRequest::getHost}, {"User-Agent:", &HttpRequest::getUserAgent},
		{"Accept:", &HttpRequest::getAccept}, {"Accept-Language:", &HttpRequest::getAcceptLanguage},
		{"Accept-Encoding:", &HttpRequest::getAcceptEncoding}, {"Connection:", &HttpRequest::getConnection},
		{"Upgrade-Insecure-Requests:", &HttpRequest::getUpInsecureRqst}, {"Referer:", &HttpRequest::getReferer},
		{"Sec-Fetch-Dest:", &HttpRequest::getSecFetchDest}, {"Sec-Fetch-Mode:", &HttpRequest::getSecFetchMode},
		{"Sec-Fetch-Site:", &HttpRequest::getSecFetchSite}, {"Content-Length:", &HttpRequest::getStrContentLength},
		{"Content-Type:", &HttpRequest::getContentType}, {"Cookie:", &HttpRequest::getCookie}};
	for (std::size_t i = 0; i < NBPARAM; i++)
		std::cout << YELLOW << tab[i].key << RESET << (this->*(tab[i].getter))() << std::endl;
}

void		HttpRequest::resetRequest(void)
{
	_statusCode = NEW_BORN;
	_method = "";
	_path = "";
	_http = "";
	_host = "";
	_userAgent = "";
	_accept = "";
	_acceptLanguage = "";
	_acceptEncoding = "";
	_connection = "";
	_upInsecureRqst = "";
	_referer = "";
	_secFetchDest = "";
	_secFetchMode = "";
	_secFetchSite = "";
	_contentLength = 0;
	_contentType = "";
	_cookie = "";

	_bodyRequest = "";
	_headerRequest = "";
	
	saveString = "";
	_strContentLength = "";

	_myServer = NULL;
	_isCgi = false;
	clock_gettime(CLOCK_REALTIME, &_keepAliveTimeout);
	clock_gettime(CLOCK_REALTIME, &_requestTimeout);
}

//-----------Core functions----------

int	HttpRequest::recvfd(int & fd)
{
	u_int8_t recvline[MAXLINE + 1];
	memset(recvline, 0, MAXLINE);
	int numbytes;
	numbytes = recv(fd, recvline, MAXLINE - 1, 0);
	saveString += reinterpret_cast< char * >(recvline); 
	if (numbytes < 0)
	{
		server_log("Request clientFd " + _debugFd + " recv error", ERROR);
		_statusCode = 400;
	}
	if (numbytes == 0)
		_statusCode = KILL_ME;
	return (numbytes);
}

int    HttpRequest::processingRequest(void)
{
	clock_gettime(CLOCK_REALTIME, &_keepAliveTimeout);
	if (_statusCode == NEW_BORN)
	{
		clock_gettime(CLOCK_REALTIME, &_requestTimeout);
		_statusCode = NEW;
	}
	if (_statusCode > 200 || _statusCode == KILL_ME)
		return (_statusCode);
	server_log("Request clientFd " + _debugFd + " starting processing...", DEBUG);
	recvfd(_connfd);
	HttpRequestParsing	parsing(*this);
	if (_statusCode == NEW || _statusCode == PROCESSING_HEADER)
	{
		server_log("Request clientFd " + _debugFd + " getHeader in process", DEBUG);
		parsing.parsingHeader();
	}
	if (_statusCode == DONE_HEADER)
	{
		HttpRequestChecking checking(*this);
		server_log("Request clientFd " + _debugFd + " Header has been found", DEBUG);
		_myServer = checking.findMyServer(_servers);
		if (_myServer == NULL || checking.BuildAndCheckHeader() != 0)
			return (_statusCode);
		else
		{
			_statusCode = DONE_HEADER_CHECKING;
			server_log("Request clientFd " + _debugFd + " checking Header succeed", DEBUG);
			server_log("Request clientFd " + _debugFd + " server name : " + _myServer->getServerName(), DEBUG);
			server_log("Request clientFd " + _debugFd + " final path : " + _path , DEBUG);
		}
	}
	if (_statusCode == DONE_HEADER_CHECKING || _statusCode == PROCESSING_BODY)
	{
		if (parsing.parsingBody() == true)
			server_log("Request clientFd " + _debugFd + " succesful status code " + int_to_str(_statusCode), INFO);
	}
	return (_statusCode);
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
std::string	HttpRequest::getCookie()			{return (this->_cookie);}

std::string HttpRequest::getBodyRequest()		{return (this->_bodyRequest);}
std::string HttpRequest::getHeaderRequest()		{return (this->_headerRequest);}

int			HttpRequest::getConnfd()			{return (this->_connfd);}
std::string HttpRequest::getSaveString()		{return (this->saveString);}

std::string HttpRequest::getStrContentLength()	{return (this->_strContentLength);}
int			HttpRequest::getStatusCode()		{return (this->_statusCode);}
Server		*HttpRequest::getMyserver()			{return (this->_myServer);}

bool		HttpRequest::getIsCgi()				{return (this->_isCgi);}

int			HttpRequest::getListenFd()			{return (this->_listenFd);}

std::size_t	HttpRequest::getMaxBodySize()		{return (this->_maxBodySize);}

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
void	HttpRequest::setCookie(const std::string &value)			{_cookie = value;}

void	HttpRequest::setBodyRequest(const std::string &value)		{_bodyRequest = value;}
void	HttpRequest::setHeaderRequest(const std::string &value)		{_headerRequest = value;}

void	HttpRequest::setConnfd(const int &value)					{_connfd = value;}
void	HttpRequest::setSaveString(const std::string &value)		{saveString = value;}

void	HttpRequest::setStrContentLength(const std::string &value)	{_strContentLength = value;}
void	HttpRequest::setStatusCode(const int &value)				{_statusCode = value;}
void	HttpRequest::setMyserver(Server **value)					{if (value == NULL){_myServer = NULL;}else{_myServer= *value;}}

void	HttpRequest::setIsCgi(const bool &value)					{_isCgi = value;}

void	HttpRequest::setMaxBodySize(const std::size_t &value)		{_maxBodySize = value;}
