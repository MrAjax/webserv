#include "ResponseSender.hpp"

ResponseSender::ResponseSender(std::string &response, HttpRequest &Req, struct pollfd &mypoll) : _response(response), _request(Req), _mypoll(mypoll)
{
    if (_request.getIsChunked() == false && isMaxSize(response))
    {
        server_log("Client fd " + int_to_str(_request.getConnfd()) + " send.size() > send Max size" + int_to_str(SEND_MAX) + " starting chunked transfer encoding", DEBUG);
        catchHeader();
        putTransfertEncoding();
        _request.setIsChunked(true);
    }
    else
        _response = "";
    if (_request.getIsChunked() == true)
        if (createChunk() == true)
            _request.setIsChunked(false);
    send_response_to_client();
    if (_request.getIsChunked() == false)
        closeRequest();
}

ResponseSender::~ResponseSender() {}

void    ResponseSender::closeRequest()
{
    if (_request.getIsChunked() == true)
        return ;
    if (_request.getConnection() == "close" || _request.getStatusCode() >= 400)
    {
        server_log("Set clientFd " + int_to_str(_request.getConnfd()) + " to close", DEBUG);
    	_request.setStatusCode(KILL_ME);
    }
	else
	{
		server_log("Reset clientFd " + int_to_str(_request.getConnfd()) + " for other requests (keep-alive)", DEBUG);
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

bool ResponseSender::createChunk()
{
    std::string strChunk;
    if (isMaxSize(_request.getSaveString()))
    {
        strChunk = _request.getSaveString().substr(0, SEND_MAX);
        _request.setSaveString(_request.getSaveString().substr(SEND_MAX));
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
    return (false);
}

int    ResponseSender::putTransfertEncoding()
{
    std::string toInsert = "Transfer-Encoding: chunked";
    std::size_t pos_toFind;
    pos_toFind = _response.find(toInsert);
    if (pos_toFind != std::string::npos)
        server_log("Transfer-Encoding: chunked already HttpRequest fd = " + int_to_str(_request.getConnfd()), DEBUG);
    std::string toFind = "Content-Length:";
    pos_toFind = _response.find(toFind);
    if (pos_toFind != std::string::npos)
    {
        std::size_t pos_end = _response.find("\r\n", pos_toFind + toFind.size());
        if (pos_end != std::string::npos)
            _response.replace(pos_toFind, (pos_toFind - pos_end), toInsert);
    }
}

bool    ResponseSender::isMaxSize(std::string const &str)
{
    if (str.size() > SEND_MAX)
        return (true);
    else
        return (false);
}

int	ResponseSender::send_response_to_client()
{
	server_log("Sending response...", DEBUG);
	ssize_t numbytes;
	numbytes = send(_request.getConnfd(), _response.c_str(), _response.size(), 0);
	if (numbytes == -1)
		server_log("Client fd " + int_to_str(_request.getConnfd()) + " send failed client has closed connexion", ERROR);
	else if (numbytes == 0)
		server_log("Client fd " + int_to_str(_request.getConnfd()) + " no bytes send", ERROR);
	else
	{
		server_log("Response sent bytes numbers = " + int_to_str(numbytes), INFO);
		return (true);
	}
	return (-1);
}

void	ResponseSender::send_response(int connfd, Server &serv ,HttpRequest &Req)
{
	std::string	response;
	server_log("Sending response to clientFd " + int_to_str(connfd) + "...", DEBUG);
	try {
		if (connfd < 0)
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
			server_log("No server in request clientFd " + int_to_str(connfd) + " status code = " + int_to_str(Req.getStatusCode()), ERROR);
			throw	StatusSender::send_status(500, serv, false);
		}
		if (Req.getHeaderRequest().empty()) {
			server_log("ClientFd " + int_to_str(connfd) + " invalid request : no header", ERROR);
			server_log("ClientFd " + int_to_str(connfd) + " request status code : " + int_to_str(Req.getStatusCode()), ERROR);
			throw	StatusSender::send_status(400, serv, true);
		}
		server_log("ClientFd " + int_to_str(connfd) + " server name is " + serv.getServerName(), DEBUG);
		server_log("Request is valid", DEBUG);
		server_log(Req.getHeaderRequest() + "\n\n", DIALOG);
		server_log(Req.getBodyRequest() + "\n\n", DIALOG);
		server_log("Building Response...", DEBUG);

		HttpResponse	Rep(Req, serv);

		response = Rep.get_response(serv);
		if (!Rep.get_header().empty())
			server_log(Rep.get_header(), DIALOG);
		else
			server_log(response, DIALOG);
		send_response_to_client(connfd, response, Req);
		if (Req.getConnection() == "close" || Req.getStatusCode() >= 400)
			Req.setStatusCode(KILL_ME);
	}
	catch (std::string &s) {
		response = s;
		server_log(Req.getHeaderRequest() + "\n\n", DIALOG);
		server_log(Req.getBodyRequest() + "\n\n", DIALOG);
		server_log(response, DIALOG);
		send_response_to_client(connfd, response, Req);
		Req.setStatusCode(KILL_ME);
	}
}

