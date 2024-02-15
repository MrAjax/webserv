#include "ResponseSender.hpp"

ResponseSender::ResponseSender(std::string &response, HttpRequest &Req) : _response(response), _request(Req)
{
    
}

ResponseSender::~ResponseSender() {}

bool ResponseSender::sendHeader()
{
    std::string toFind = "\r\n\r\n";
    std::size_t pos_toFind = _response.find(toFind);
    if (pos_toFind != std::string::npos)
    {
        _request.setSaveString(_response.substr(pos_toFind + toFind.size()));
        _response = _response.substr(0, pos_toFind + toFind.size());

    }
}

int    ResponseSender::chunked()
{
    std::string toInsert = "Transfer-Encoding: chunked";
    std::size_t pos_toFind;
    pos_toFind = _response.find(toInsert);
    if (pos_toFind != std::string::npos)
        server_log("Transfer-Encoding: chunked HttpRequest fd = " + int_to_str(Req.getConnfd()), ERROR);
    std::string toFind = "Content-Length:";
    pos_toFind = _response.find(toFind);
    if (pos_toFind != std::string::npos)
    {
        std::size_t pos_end = _response.find("\r\n", pos_toFind + toFind.size());
        if (pos_end != std::string::npos)
            _response.replace(pos_toFind, (pos_toFind - pos_end), toInsert);
    }
    
}

bool    ResponseSender::isMaxSize()
{
    if (_response.size() > SEND_MAX)
        return (true);
    else
        return (false);
}

