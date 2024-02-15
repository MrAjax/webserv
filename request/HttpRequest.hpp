#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include "../inc/webserv.hpp"
#include "../parsing/Server.hpp"
#include "../utils/utils.hpp"
#include "../response/StatusSender.hpp"

enum	{KILL_ME = -1, NEW, PROCESSING_HEADER, DONE_HEADER,
		DONE_HEADER_CHECKING, PROCESSING_BODY, DONE_ALL};

#define NBPARAM 18
#define KEEP_ALIVE_TIMEOUT 10 // en seconds
#define REQUEST_TIMEOUT 15 // en seconds

class HttpRequest
{
	public:

		HttpRequest(void);
		HttpRequest(int connfd);
		HttpRequest(int connfd, std::vector<Server> &servers, int listenFd);

		HttpRequest(HttpRequest const &copy);
		~HttpRequest(void);
		//------------OPERATOR------------------//

		int     processingRequest(void); //on recupere le header et le boby + parsing
		int		recvfd(int & fd); //on read buffer[MAXLINE -1] du fd

		//------------UTIL----------------------
		bool		isKeepAliveTimeout(void);
		bool		isRequestTimeout(void);
		void		printAttribute(void);
		void		resetRequest(void);
		//---------Guetter-----------------
		std::string getMethod();
		std::string getPath();
		std::string getHttp();
		std::string getHost();
		std::string getUserAgent();
		std::string getAccept();
		std::string getAcceptLanguage();
		std::string getAcceptEncoding();
		std::string getConnection();
		std::string getUpInsecureRqst();
		std::string getReferer();
		std::string getSecFetchDest();
		std::string getSecFetchMode();
		std::string getSecFetchSite();
		ssize_t		getContentLength();
		std::string getContentType();
		std::string getCookie();
		std::string getTransferEncoding();

		std::string getBodyRequest();
		std::string getHeaderRequest();

		int         getConnfd();
		std::string getSaveString();
		std::string getStrContentLength();

		int			getStatusCode();
		bool		getIsCgi();
		bool		getIsChunked();


		Server		*getMyserver();

		int			getListenFd();

		std::size_t	getMaxBodySize();
		
		// ----------------------Setter--------------

		void	setMethod(const std::string &value);
		void	setPath(const std::string &value);
		void	setHttp(const std::string &value);
		void	setHost(const std::string &value);
		void	setUserAgent(const std::string &value);
		void	setAccept(const std::string &value);
		void	setAcceptLanguage(const std::string &value);
		void	setAcceptEncoding(const std::string &value);
		void	setConnection(const std::string &value);
		void	setUpInsecureRqst(const std::string &value);
		void	setReferer(const std::string &value);
		void	setSecFetchDest(const std::string &value);
		void	setSecFetchMode(const std::string &value);
		void	setSecFetchSite(const std::string &value);
		void	setContentLength(const ssize_t &value);
		void	setContentType(const std::string &value);
		void	setCookie(const std::string &value);
		void	setTransferEncoding(const std::string &value);

		void	setBodyRequest(const std::string &value);
		void	setHeaderRequest(const std::string &value);

		void	setConnfd(const int &value);

		void	setSaveString(const std::string & value);

		void	setStrContentLength(const std::string &value);

		void	setStatusCode(const int &value);
		void	setIsCgi(const bool &value);
		void	setIsChunked(const bool &value);

		void	setMaxBodySize(const std::size_t &value);

		void	setMyserver(Server **value);

	private:
		std::string _method;
		std::string _path;
		std::string _http;
		std::string _host;
		std::string _userAgent;
		std::string _accept;
		std::string _acceptLanguage;
		std::string _acceptEncoding;
		std::string _connection;
		std::string _upInsecureRqst;
		std::string _referer;
		std::string _secFetchDest;
		std::string _secFetchMode;
		std::string _secFetchSite;
		ssize_t		_contentLength;
		std::string _contentType;
		std::string _cookie;
		std::string _transferEncoding;

		std::string _bodyRequest;
		std::string _headerRequest;
		
		int         _connfd;
		std::string saveString;
		std::string _strContentLength;

		std::vector<Server> &_servers;
		Server				*_myServer;

		int         		_statusCode;

		bool				_isCgi;
		bool				_isChunked;
		int 				_listenFd;

		std::size_t			_maxBodySize;

		std::string			_debugFd;

		struct timespec _keepAliveTimeout;
		struct timespec _requestTimeout;

};

#endif
