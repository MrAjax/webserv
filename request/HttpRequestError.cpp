#include "HttpRequestError.hpp"

#define ROOT "/Zzewebsite/index/chien.jpeg"
#define MAXSIZE 100000

HttpRequestError::HttpRequestError(HttpRequest &request) : _request(request)
{
    
}

HttpRequestError::~HttpRequestError() {}

int HttpRequestError::GET(void)
{
    // if (_request.getPath() == "/")
    //     _request.getPath() = ROOT;
    // int status = access(_request.getPath().c_str(), F_OK);
    // if (status < 0)
    // {
    //     std::cout << "ACCESSSSS TESSSSSSSSST\n";
    //     return (1);
    // }
    if (protocol() > 0)
        return (4);
    return (0);
}

int HttpRequestError::POST(void)
{
    if (_request.getContentType().empty())
        return (1);
    if (_request.getContentLength() == 0)
        return (2);
    int status = access(_request.getPath().c_str(), F_OK);
    if (status < 0)
        return (3);
    if (protocol() > 0)
        return (4);
    return (0);   
}

int HttpRequestError::DELETE(void)
{
    if (_request.getPath().rfind("/monsite/", 0) == std::string::npos)
        return (2);
    int status = access(_request.getPath().c_str(), F_OK);
    if (status < 0)
        return (3);
    if (protocol() > 0)
        return (4);
    return (1);
}

int HttpRequestError::Method(void)
{
    int         (HttpRequestError::*f[3])(void) = {&HttpRequestError::GET, &HttpRequestError::POST, &HttpRequestError::DELETE};
    std::string tabMethod[3] = {"GET", "POST", "DELETE"};
    int choice = 0;
    for (;choice < 3; choice++)
    {
        if (tabMethod[choice] == _request.getMethod())
            break ;
    }
    switch (choice)
    {
        case 0:
            (this->*f[0])();
            break;
        case 1:
            (this->*f[1])();
            break;
        case 2:
            (this->*f[2])();
            break;
        case 3:
			throw std::runtime_error("Error: No good method, try GET POST or DELETE\n");
            return (1);
            break;
    }
    return (0);
}

int HttpRequestError::protocol(void)
{
    if (_request.getHttp() != "HTTP/1.1")
        return (1);
    return (0);
}

int HttpRequestError::maxSize(void)
{
    if (_request.getSaveString().size() > MAXSIZE)
        return (1);
    else if (_request.getHeaderRequest().size() + _request.getBodyRequest().size() > MAXSIZE)
        return (2);
    return (0);
}