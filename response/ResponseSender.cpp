#include "ResponseSender.hpp"

ResponseSender::ResponseSender(HttpRequest &Req, struct pollfd &mypoll, Server &serv) : _response(""), _request(Req), _mypoll(mypoll)
{
    server_log("Sending response to clientFd " + int_to_str(mypoll.fd) + "...", DEBUG);
    if (Req.getIsChunked() == true)
    {
        processingChunk();
        return ;
    }

	try {
		if (mypoll.fd < 0)
		{
			server_log("pollfds[i].fd = -1 HttpRequest fd = " + int_to_str(Req.getConnfd()), ERROR);
			throw	StatusSender::send_status(500, serv, false);
		}
		if (Req.getMyserver() == NULL && Req.getStatusCode() >= 400)
			throw	StatusSender::send_status(Req.getStatusCode(), serv, false);
		else if (Req.getStatusCode() >= 400)
			throw	StatusSender::send_status(Req.getStatusCode(), serv, true);
		else if (Req.getMyserver() == NULL)
		{
			server_log("No server in request clientFd " + int_to_str(mypoll.fd) + " status code = " + int_to_str(Req.getStatusCode()), ERROR);
			throw	StatusSender::send_status(500, serv, false);
		}
		if (Req.getHeaderRequest().empty()) {
			server_log("ClientFd " + int_to_str(mypoll.fd) + " invalid request : no header", ERROR);
			server_log("ClientFd " + int_to_str(mypoll.fd) + " request status code : " + int_to_str(Req.getStatusCode()), ERROR);
			throw	StatusSender::send_status(400, serv, true);
		}
		server_log("ClientFd " + int_to_str(mypoll.fd) + " server name is " + serv.getServerName(), DEBUG);
		server_log("Request is valid", DEBUG);
		server_log(Req.getHeaderRequest() + "\n\n", DIALOG);
		server_log(Req.getBodyRequest() + "\n\n", DIALOG);
		server_log("Building Response...", DEBUG);

		HttpResponse	Rep(Req, serv);

		_response = Rep.get_response(serv);
		if (!Rep.get_header().empty())
			server_log(Rep.get_header(), DIALOG);
		else
			server_log(_response, DIALOG);
		send_first_response_to_client();
	}
	catch (std::string &s) {
		_response = s;
		server_log(Req.getHeaderRequest() + "\n\n", DIALOG);
		server_log(Req.getBodyRequest() + "\n\n", DIALOG);
		server_log(_response, DIALOG);
		send_first_response_to_client();
	}
}

ResponseSender::~ResponseSender() {}

void    ResponseSender::processingChunk()
{
    server_log("Sending next chunk to clientFd " + int_to_str(_mypoll.fd), DEBUG);
    if (createChunk() == true)
        _request.setIsChunked(false);
    if (send_response_to_client() == false)
        return (_request.setStatusCode(KILL_ME));
    if (_request.getIsChunked() == false)
        closeRequest();
}

void    ResponseSender::closeRequest()
{
    if (_request.getIsChunked() == true || _request.getStatusCode() == KILL_ME)
        return ;
    if (_request.getConnection() == "close" || _request.getStatusCode() >= 400)
    {
        server_log("Set clientFd " + int_to_str(_mypoll.fd) + " to close", DEBUG);
    	_request.setStatusCode(KILL_ME);
    }
	else
	{
		server_log("Reset clientFd " + int_to_str(_mypoll.fd) + " for other requests (keep-alive)", DEBUG);
		_mypoll.events = POLLIN;
		_mypoll.revents = 0;
		_request.resetRequest();
	}
}

std::string chunk(std::string &str)
{
    std::string delimiteur = "\r\n";
    std::string sizeHexa;
    std::string response;
    std::stringstream ss;
    ss << std::hex << str.size();
    ss >> sizeHexa;
    response = sizeHexa + delimiteur + str + delimiteur;
    return (response);
}

std::size_t	sizeConvert(std::size_t strSize)
{
    std::size_t hexaSize = 1;
    for(std::size_t i = 16; i < strSize; i *= 16)
        hexaSize++;
    return (hexaSize);
}

bool ResponseSender::createChunk()
{
    std::string strChunk;
    std::size_t maxLine = (SEND_MAX - 4);
    std::size_t temp = sizeConvert(maxLine);
    if (sizeConvert(maxLine - temp) == temp)
        maxLine -= temp;
    else
        maxLine -= temp -1;
    if (maxLine < _request.getSaveString().size() + 5)
    {
        strChunk = _request.getSaveString().substr(0, maxLine);
        _request.setSaveString(_request.getSaveString().substr(maxLine));
        _response += chunk(strChunk);
        return (false);
    }
    else
    {
        strChunk = _request.getSaveString();
        _request.setSaveString("");
        _response += chunk(strChunk);
        _response += "0\r\n\r\n";
        return (true);
    }
}


bool ResponseSender::catchHeader()
{
    std::string toFind = "\r\n\r\n";
    std::size_t pos_toFind = _response.find(toFind);
    if (pos_toFind != std::string::npos)
    {
        _request.setSaveString(_response.substr(pos_toFind + toFind.size()));
        _response = _response.substr(0, pos_toFind + toFind.size());
        return (true);
    }
    server_log("ResponseSender can't catch response header clientFd " + int_to_str(_mypoll.fd), ERROR);
    return (false);
}

void    ResponseSender::putTransfertEncoding()
{
    std::string toInsert = "Transfer-Encoding: chunked";
    std::size_t pos_toFind;
    pos_toFind = _response.find(toInsert);
    if (pos_toFind != std::string::npos)
    {
        server_log("Transfer-Encoding: chunked already in response clientFd " + int_to_str(_mypoll.fd), DEBUG);
        return ;
    }
    std::string toFind = "Content-Length:";
    pos_toFind = _response.find(toFind);
    if (pos_toFind != std::string::npos)
    {
        std::size_t pos_end = _response.find("\r\n", pos_toFind + toFind.size());
        if (pos_end != std::string::npos)
            _response.replace(pos_toFind, (pos_toFind - pos_end), toInsert);
    }
}

bool    ResponseSender::isMaxSize(std::size_t const &size)
{
    if (size > SEND_MAX)
        return (true);
    else
        return (false);
}

void	ResponseSender::send_first_response_to_client()
{
    if (_request.getIsChunked() == false && isMaxSize(_response.size()))
    {
        server_log("Client fd " + int_to_str(_mypoll.fd) + " send.size() > send Max size" + int_to_str(SEND_MAX) + " starting chunked transfer encoding", DEBUG);
        if (catchHeader() == false)
            return (_request.setStatusCode(KILL_ME));
        putTransfertEncoding();
        _request.setIsChunked(true);
    }
    if (_request.getIsChunked() == true)
        if (createChunk() == true)
            _request.setIsChunked(false);
    if (send_response_to_client() == false)
        return (_request.setStatusCode(KILL_ME));
    if (_request.getIsChunked() == false)
        closeRequest();
}

// int	ResponseSender::send_response_to_client()
// {
// 	server_log("Sending response...", DEBUG);
// 	ssize_t numbytes;
// 	numbytes = send(_mypoll.fd, _response.c_str(), _response.size(), 0);
// 	if (numbytes == -1)
//     {
// 		server_log("Client fd " + int_to_str(_mypoll.fd) + " send failed client has closed connexion", ERROR);
//         return (false);
//     }
// 	else if (numbytes == 0)
//     {
// 		server_log("Client fd " + int_to_str(_mypoll.fd) + " no bytes send", ERROR);
//         return (false);
//     }
// 	server_log("Response sent bytes numbers = " + int_to_str(numbytes), INFO);
// 	return (true);
// }

// void	ResponseSender::send_response(int connfd, Server &serv, HttpRequest &Req)
// {
// 	server_log("Sending response to clientFd " + int_to_str(connfd) + "...", DEBUG);
//     if (Req.getIsChunked() == true)
//         return (processingChunk());

// 	try {
// 		if (connfd < 0)
// 		{
// 			server_log("pollfds[i].fd = -1 HttpRequest fd = " + int_to_str(Req.getConnfd()), ERROR);
// 			throw	StatusSender::send_status(500, serv, false);
// 		}
// 		if (Req.getMyserver() == NULL && Req.getStatusCode() >= 400)
// 			throw	StatusSender::send_status(Req.getStatusCode(), serv, false);
// 		else if (Req.getStatusCode() >= 400)
// 			throw	StatusSender::send_status(Req.getStatusCode(), serv, true);
// 		else if (Req.getMyserver() == NULL)
// 		{
// 			server_log("No server in request clientFd " + int_to_str(connfd) + " status code = " + int_to_str(Req.getStatusCode()), ERROR);
// 			throw	StatusSender::send_status(500, serv, false);
// 		}
// 		if (Req.getHeaderRequest().empty()) {
// 			server_log("ClientFd " + int_to_str(connfd) + " invalid request : no header", ERROR);
// 			server_log("ClientFd " + int_to_str(connfd) + " request status code : " + int_to_str(Req.getStatusCode()), ERROR);
// 			throw	StatusSender::send_status(400, serv, true);
// 		}
// 		server_log("ClientFd " + int_to_str(connfd) + " server name is " + serv.getServerName(), DEBUG);
// 		server_log("Request is valid", DEBUG);
// 		server_log(Req.getHeaderRequest() + "\n\n", DIALOG);
// 		server_log(Req.getBodyRequest() + "\n\n", DIALOG);
// 		server_log("Building Response...", DEBUG);

// 		HttpResponse	Rep(Req, serv);

// 		_response = Rep.get_response(serv);
// 		if (!Rep.get_header().empty())
// 			server_log(Rep.get_header(), DIALOG);
// 		else
// 			server_log(_response, DIALOG);
// 		send_first_response_to_client();
// 	}
// 	catch (std::string &s) {
// 		_response = s;
// 		server_log(Req.getHeaderRequest() + "\n\n", DIALOG);
// 		server_log(Req.getBodyRequest() + "\n\n", DIALOG);
// 		server_log(_response, DIALOG);
// 		send_first_response_to_client();
// 	}
// }

