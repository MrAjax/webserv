#ifndef SPAN_HPP
# define SPAN_HPP

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
        HttpRequest(void);
        HttpRequest(HttpRequest const &copy);
        ~HttpRequest(void);
        //OPERATOR//
        HttpRequest	&operator=(HttpRequest const &rhs);

        void        parsingHeader(int connfd);
        std::string getHeader(std::string &fullRequest);

        //---------Header parser--------------
        void        parsingHeader(std::string &header);
        void        parsingHeader_method_pathcmd_http(std::string &line);
        void        parsingHeader_host(std::string &line);
        void        parsingHeader_userAgent(std::string &line);
        void        parsingHeader_accept(std::string &line);
        void        parsingHeader_acceptLanguage(std::string &line);
        void        parsingHeader_acceptEncoding(std::string &line);
        void        parsingHeader_connection(std::string &line);
        void        parsingHeader_referer(std::string &line);
        void        parsingHeader_secFetchDest(std::string &line);
        void        parsingHeader_secFetchMode(std::string &line);
        void        parsingHeader_secFetchSite(std::string &line);
        //---------

    private:
        std::string _method;
        std::string _pathCmd;
        std::string _http;
        std::string _host;
        std::string _userAgent;
        std::string _accept;
        std::string _acceptLanguage;
        std::string _acceptEncoding;
        std::string _connection;
        std::string _referer;
        std::string _secFetchDest;
        std::string _secFetchMode;
        std::string _secFetchSite;


        std::vector< std::pair<int, bool> > _requestList; 

        
};

#endif