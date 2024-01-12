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
        void        parseAllAttributes(std::string & header);

        std::string getHeader(std::string &fullRequest);

        //---------Header parser--------------
        void        parsingHeader(std::string &header);
        void        parsingHeader_method_pathcmd_http(std::string &line);
        std::string parsingHeader_rest(std::string &line, std::string const & keyWord);
        //---------Utils---------------------
        std::string findLine(std::string header, std::string &delimiteur, std::size_t & end_pos);

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