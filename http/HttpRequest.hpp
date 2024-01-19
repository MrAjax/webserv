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

# define RESET	"\e[0m"
# define RED	"\e[31m"
# define GREEN	"\e[32m"
# define YELLOW	"\e[33m"
# define BLUE	"\e[34m"
# define PURPLE	"\e[35m"
# define CYAN	"\e[36m"


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
		HttpRequest(struct pollfd request); //TEST

		HttpRequest(int connfd, std::string contentType, std::string input);
		HttpRequest(HttpRequest const &copy);
		~HttpRequest(void);
		//------------OPERATOR------------------//
 
		void        parseAllAttributes(std::string header);

		int     processingRequest(struct pollfd &request); //on recupere le header et le boby + parsing
		void	recvfd(int & fd); //on read buffer[MAXLINE -1] du fd


		void    parsingHeader(void);
		void    parsingBody(void);


		//---------Header parser--------------
		void        splitHeaderBody(std::string &fullRequest);
		void        parsingHeader(std::string &header);
		void        parsingHeader_method_path_http(std::string &line);
		std::string parsingHeader_rest(std::string &line, std::string const & keyWord);

		//---------check error---------------------
		void        checkError();
		//---------Utils---------------------
		std::size_t findLine(std::string &header, std::string &line, std::string &delimiteur);
		std::size_t convert(std::string &toConvert);

		//---------Guetteurs-----------------
		std::string getMethod();
		std::string getPath();
		std::string getHttp();
		std::string getHost();
		std::string getUserAgent();
		std::string getAccept();
		std::string getAcceptLanguage();
		std::string getConnection();
		std::string getUpInsecureRqst();
		std::string getReferer();
		std::string getSecFetchDest();
		std::string getSecFetchMode();
		std::string getSecFetchSite();
		std::string getContentLength();
		std::string getContentType();

		std::string getBodyRequest();
		std::string getHeaderRequest();

		int         getConnfd();
		std::string getInput();
		

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
		std::string _contentLength;
		std::string _contentType;

		std::string _bodyRequest;
		std::string _headerRequest;
		
		int         _connfd;
		std::string _input;

		int         STATUS;
		std::string fullRequest;

};

#endif