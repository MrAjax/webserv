#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <sstream>

# define RESET	"\e[0m"
# define RED	"\e[31m"
# define GREEN	"\e[32m"
# define YELLOW	"\e[33m"
# define BLUE	"\e[34m"
# define PURPLE	"\e[35m"
# define CYAN	"\e[36m"

class HttpRequest
{
    public:
        HttpRequest(int connfd);
        HttpRequest(int connfd, std::string contentType, std::string input);
        HttpRequest(HttpRequest const &copy);
        ~HttpRequest(void);
        //------------OPERATOR------------------//
        HttpRequest	&operator=(HttpRequest const &rhs);

        void        parsingHeader(int connfd);
        void        parseAllAttributes(std::string header);

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

        std::string getBodyRequest();
        std::string getHeaderRequest();

        int         getConnfd();
        std::string getContentType();
        std::string getInput();

        //---------Header parser--------------
        void        splitHeaderBody(std::string &fullRequest);
        void        parsingHeader(std::string &header);
        void        parsingHeader_method_path_http(std::string &line);
        std::string parsingHeader_rest(std::string &line, std::string const & keyWord);
        //---------check error---------------------
	    void        checkError();
        //---------Utils---------------------
        std::size_t findLine(std::string &header, std::string &line, std::string &delimiteur);

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

        std::string _bodyRequest;
        std::string _headerRequest;
        
        int         _connfd;
        std::string _contentType;
        std::string _input;
};

#endif