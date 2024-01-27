#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <limits>
#include "../parsing/Server.hpp"

#define KILL_ME -1
#define NEW 0
#define PROCESSING_HEADER 1
#define DONE_HEADER 2
#define PROCESSING_BODY 3
#define DONE_ALL 4

class HttpRequest
{
	public:

		HttpRequest(void);
		HttpRequest(int connfd);
		HttpRequest(int connfd, std::vector<Server> &servers);

		HttpRequest(int connfd, std::string contentType, std::string input);
		HttpRequest(HttpRequest const &copy);
		~HttpRequest(void);
		//------------OPERATOR------------------//

		int     processingRequest(void); //on recupere le header et le boby + parsing
		int		recvfd(int & fd); //on read buffer[MAXLINE -1] du fd

		//------------UTIL----------------------
		int			checkTimeout(void);
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
		std::size_t getContentLength();
		std::string getContentType();

		std::string getBodyRequest();
		std::string getHeaderRequest();

		int         getConnfd();
		std::string getSaveString();
		std::string getStrContentLength();

		int         STATUS;


		// ----------------------Setter--------------

		void setMethod(const std::string &value);
		void setPath(const std::string &value);
		void setHttp(const std::string &value);
		void setHost(const std::string &value);
		void setUserAgent(const std::string &value);
		void setAccept(const std::string &value);
		void setAcceptLanguage(const std::string &value);
		void setAcceptEncoding(const std::string &value);
		void setConnection(const std::string &value);
		void setUpInsecureRqst(const std::string &value);
		void setReferer(const std::string &value);
		void setSecFetchDest(const std::string &value);
		void setSecFetchMode(const std::string &value);
		void setSecFetchSite(const std::string &value);
		void setContentLength(const std::size_t &value);
		void setContentType(const std::string &value);

		void setBodyRequest(const std::string &value);
		void setHeaderRequest(const std::string &value);

		void setConnfd(const int &value);

		void setSaveString(const std::string & value);

		void setStrContentLength(const std::string &value);

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
		std::size_t _contentLength;
		std::string _contentType;

		std::string _bodyRequest;
		std::string _headerRequest;
		
		int         _connfd;
		std::string saveString;
		std::string _strContentLength;
		struct timespec _lastUpdate;

		std::vector<Server> &_servers;

};

#endif