#ifndef SPAN_HPP
# define SPAN_HPP

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <vector>

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

        void parsingHeader(int connfd);

    private:
        std::string _cmd;
        std::string _protocol;
        std::string _ContentType;
        std::string _ContentLength;
        std::string _Content;

        std::vector< std::pair<int, bool> > _requestList; 

        
};

#endif