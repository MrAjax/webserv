#include "ResponseSender.hpp"

ResponseSender::ResponseSender(std::string &response) : _response(response)
{
    
}

ResponseSender::~ResponseSender() {}

int    ResponseSender::chunked()
{
    std::size_t pos;
    pos = _response.find("Content-Length:");
}

bool    ResponseSender::isMaxSize()
{
    if (_response.size() > SEND_MAX)
        return (true);
    else
        return (false);
}

