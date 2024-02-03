#include "HttpRequestChecking.hpp"

#define MAX_HEADER_SIZE 1000

HttpRequestChecking::HttpRequestChecking(HttpRequest &request) : _request(request)
{
	std::stringstream ss;
	ss << _request.getConnfd();
	_strFd = ss.str();
}

HttpRequestChecking::~HttpRequestChecking() {}


int HttpRequestChecking::GET(void)
{
	findCgi();
	isGoodProtocol("HTTP/1.1");
	if (!findRootPath() && !findOtherPath())
		return (1);
	return (0);
}

int HttpRequestChecking::POST(void)
{
	findCgi();
	isGoodProtocol("HTTP/1.1");
	if (_request.getContentType().empty())
	{
		server_log(std::string(REDD) + "Reqest fd " + _strFd + " method POST whitout content type", ERROR);
		_request.setStatusCode(411);
		return (2);
	}
	if (_request.getContentLength() == 0)
	{
		server_log(std::string(REDD) + "Reqest fd " + _strFd + " method POST whitout content lenght", ERROR);
		_request.setStatusCode(411);
		return (3);
	}
	if (!findRootPath() && !findOtherPath())
		return (4);
	return (0);   
}

int HttpRequestChecking::DELETE(void)
{
	findCgi();
	isGoodProtocol("HTTP/1.1");
	if (!findRootPath() && !findOtherPath())
		return (5);
	return (0);
}

int HttpRequestChecking::BuildAndCheckHeader(void)
{
	if (_request.getMyserver() == NULL)
		return (6);
	_request.setMaxBodySize(_request.getMyserver()->getMaxBodySize());
	int		(HttpRequestChecking::*f[3])(void) = {&HttpRequestChecking::GET, 
				&HttpRequestChecking::POST, &HttpRequestChecking::DELETE};
	std::string tabMethod[3] = {"GET", "POST", "DELETE"};
	int choice = 0;
	for (;choice < 3; choice++)
		if (tabMethod[choice] == _request.getMethod())
			break ;
	switch (choice)
	{
		case 0:
			return ((this->*f[0])());
		case 1:
			return ((this->*f[1])());
		case 2:
			return ((this->*f[2])());
		case 3:
			server_log(std::string(REDD) + "Request fd " + _strFd + " method "
				+ _request.getMethod() + " not allowed", ERROR);
			_request.setStatusCode(405);
			return (7);
	}
	return (8);
}

bool HttpRequestChecking::isGoodProtocol(std::string const &http)
{
	if (_request.getHttp() == http)
		return (true);
	server_log(std::string(REDD) + "Request fd " + _strFd + " protocol "
		+ _request.getHttp() + " not allowed", ERROR);
	_request.setStatusCode(400);
	return (false);
}

int HttpRequestChecking::maxSize(void)
{
	if (_request.getMyserver() == NULL)
		return (3);
	if (_request.getSaveString().size() + _request.getBodyRequest().size() > _request.getMaxBodySize())
		return (1);
	if (_request.getHeaderRequest().size() > MAX_HEADER_SIZE)
		return (2);
	return (0);
}

int HttpRequestChecking::socketfdServers(std::vector<Server> &servers)
{
	std::vector<Server>::iterator it = servers.begin();
	for (; it != servers.end(); it++)
		if (it->getSocketfd() == _request.getConnfd())
			return (1);
	return (0);
}

int HttpRequestChecking::checkSockfdPortIP(Server &server)
{
	std::size_t pos = _request.getHost().find(":");
	if (pos == std::string::npos)
		return (0);
	std::string ip = _request.getHost().substr(0, pos);
	std::string port = _request.getHost().substr(1 + pos);
	std::string convertIP = server.getIp();
	if (convertIP == "127.0.0.1" || convertIP == "::1")
		convertIP = "localhost";
	if (ip == convertIP && port == server.getPort()
		&& _request.getListenFd() == server.getSocketfd())
		return (1);
	return (0);
}

Server  *HttpRequestChecking::findMyServer(std::vector<Server> &servers)
{
	Server  *findServer = NULL;

	std::vector<Server>::reverse_iterator it = servers.rbegin();
	for (; it != servers.rend(); it++)
	{
		if (checkSockfdPortIP(*it))
			findServer = &(*it);
	}
	return (findServer);
}

bool HttpRequestChecking::findCgi()
{
	if (!is_cgi(_request.getPath()))
		return (false);
	_request.setIsCgi(true);
	_request.setPath(trimString(_request.getPath(), ".cgi", END));
	server_log(std::string(GREEN) + "Request fd " + _strFd + " find cgi", DEBUG);
	return (true);
}

//------------------ find PATHS -------------------------------

bool HttpRequestChecking::findRootPath()
{
	if (_request.getPath() != "/")
		return (false);
	int status = -1;
	std::string finalPath;
	std::vector<std::string> temp = _request.getMyserver()->getIndex();
	std::vector<std::string>::iterator it = temp.begin();
	for (;it != temp.end(); it++)
	{
		std::string tempStr = trimString(*it, "/", START);
		finalPath = trimString(_request.getMyserver()->getRoot() + "/" + tempStr, "/", START);
		int check = isGoodPath(finalPath);
		if (check == true)
		{
			_request.setPath(finalPath);
			return (true);
		}
		else if (check == 0)
			status = 0;
	}
	if (status == -1)
	{
		_request.setStatusCode(404);
		server_log(std::string(REDD) + "Request fd " + _strFd + " cannot find any valide path root/index", ERROR);
	}
	if (status == 0)
	{
		_request.setStatusCode(403);
		server_log(std::string(REDD) + "Request fd " + _strFd + " cannot access any path root/index", ERROR);
	}
	return (false);
}

bool HttpRequestChecking::findOtherPath()
{
	if (_request.getPath() == "/")
		return (false);
	std::string tempStr = trimString(_request.getPath(), "/", START);
	std::string finalPath;
	finalPath = trimString(_request.getMyserver()->getRoot() + "/" + tempStr, "/", START);
	int status = isGoodPath(finalPath);
	if (status == true)
	{
		_request.setPath(finalPath);
		return (true);
	}
	else if (status == -1)
	{
		_request.setStatusCode(404);
		server_log(std::string(REDD) + "Request fd " + _strFd + " cannot find the path : " + finalPath, ERROR);
	}
	else if (status == 0)
	{
		_request.setStatusCode(403);
		server_log(std::string(REDD) + "Request fd " + _strFd + " cannot access path : " + finalPath, ERROR);
	}
	return (false);
}

//----------------Checking PATHS ----------------------

int	HttpRequestChecking::isGoodPath(std::string &str)
{
	int status = access(str.c_str(), R_OK);
	if (status == -1)
		return (-1);
	std::string tabMethod[3] = {"GET", "POST", "DELETE"};
	int choice = 0;
	for (;choice < 3; choice++)
		if (tabMethod[choice] == _request.getMethod())
			break ;
	switch (choice)
	{
		case 0:
			return (hasReadPermission(str));
		case 1:
			return (hasWritePermission(str));
		case 2:
			return (hasReadPermission(str) && hasWritePermission(str) && hasExecutePermission(str));
		case 3:
			throw error_throw("Request fd " + _strFd + " method not allowed", false); //TODO 
			return (false);
	}
	return (false);
}

bool    HttpRequestChecking::hasReadPermission(std::string &str)
{
	int status = access(str.c_str(), R_OK);
	if (status < 0)
		return (false);
	else
		return (true);
}

bool    HttpRequestChecking::hasWritePermission(std::string &str)
{
	int status = access(str.c_str(), W_OK);
	if (status < 0)
		return (false);
	else
		return (true);
}

bool    HttpRequestChecking::hasExecutePermission(std::string &str)
{
	int status = access(str.c_str(), X_OK);
	if (status < 0)
		return (false);
	else
		return (true);
}
