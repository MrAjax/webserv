#ifndef SPAN_HPP
# define SPAN_HPP

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

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

        int parsingHeader();

        std::string getMethod();
        std::string getPath();
        std::string getProtocol();
        std::string getHeader();

    private:
        std::string cmd;
        std::string protocol;
        std::string ContentType;
        std::string ContentLength;
        std::string Content;
        
};

#endif