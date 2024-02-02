#include "HttpRequestError.hpp"

#define ROOT "/Zzewebsite/index/chien.jpeg"
#define MAXSIZE 100000

HttpRequestError::HttpRequestError(HttpRequest &request) : _request(request)
{
	std::stringstream ss;
	ss << _request.getConnfd();
	_strFd = ss.str();
}

HttpRequestError::~HttpRequestError() {}

int HttpRequestError::GET(void)
{
	return (0);
}

int HttpRequestError::POST(void)
{
	if (_request.getContentType().empty())
	{
		server_log(std::string(REDD) + "Reqest fd " + _strFd + " method POST whitout content type", ERROR);
		return (1);
	}
	if (_request.getContentLength() == 0)
	{
		server_log(std::string(REDD) + "Reqest fd " + _strFd + " method POST whitout content lenght", ERROR);
		return (2);
	}
	return (0);   
}

int HttpRequestError::DELETE(void)
{
	return (0);
}

int HttpRequestError::Method(void)
{
	// if (_request.getMyserver() == NULL)
	// 	return (false);
	int         (HttpRequestError::*f[3])(void) = {&HttpRequestError::GET, &HttpRequestError::POST, &HttpRequestError::DELETE};
	std::string tabMethod[3] = {"GET", "POST", "DELETE"};
	int choice = 0;
	for (;choice < 3; choice++)
		if (tabMethod[choice] == _request.getMethod())
			break ;
	switch (choice)
	{
		case 0:
			(this->*f[0])();
			break;
		case 1:
			if ((this->*f[1])() != 0)
				server_log(std::string(REDD) + "Passage " + _strFd + " cannot find any valide path root/index", ERROR);
			break;
		case 2:
			(this->*f[2])();
			break;
		case 3:
			server_log(std::string(REDD) + "Request fd " + _strFd + "method not allowed", ERROR);
			_request.setStatusCode(405);
			return (1);
			break;
	}
	return (0);
}

bool HttpRequestError::isGoodProtocol(void)
{
	if (_request.getHttp() == "HTTP/1.1")
		return (true);
    else
	    return (false);
}

int HttpRequestError::maxSize(void)
{
	if (_request.getSaveString().size() > MAXSIZE)
		return (1);
	else if (_request.getHeaderRequest().size() + _request.getBodyRequest().size() > MAXSIZE)
		return (2);
	return (0);
}

int HttpRequestError::socketfdServers(std::vector<Server> &servers)
{
	std::vector<Server>::iterator it = servers.begin();
	for (; it != servers.end(); it++)
		if (it->getSocketfd() == _request.getConnfd())
			return (1);
	return (0);
}

int HttpRequestError::checkPortIP(Server &server)
{
	std::size_t pos = _request.getHost().find(":");
	if (pos == std::string::npos)
		return (0);
	std::string ip = _request.getHost().substr(0, pos);
	std::string port = _request.getHost().substr(1 + pos);
	std::string convertIP = server.getIp();
	if (convertIP == "127.0.0.1")
		convertIP = "localhost";
	if (ip == convertIP && port == server.getPort())
		return (1);
	return (0);
}

Server  *HttpRequestError::findMyServer(std::vector<Server> &servers)
{
	Server  *findServer = NULL;

	std::vector<Server>::reverse_iterator it = servers.rbegin();
	for (; it != servers.rend(); it++)
	{
		if (checkPortIP(*it))
		{
			findServer = &(*it);
		}
	}
	return (findServer);
}

bool HttpRequestError::findRootPath()
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

bool HttpRequestError::findOtherPath()
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

bool HttpRequestError::getFinalPath(void)
{
	if (_request.getMyserver() == NULL)
		return (false);
	if (is_cgi(_request.getPath()))
	{
		_request.setIsCgi(true);
		_request.setPath(trimString(_request.getPath(), ".cgi", END));
	}
	int status = -1;
	std::stringstream ss;
	ss << _request.getConnfd();
	std::string finalPath;
	std::string tempStr;
	if (_request.getPath() == "/")
	{
		std::vector<std::string> temp = _request.getMyserver()->getIndex();
		std::vector<std::string>::iterator it = temp.begin();
		for (;it != temp.end(); it++)
		{
			tempStr = trimString(*it, "/", START);
			finalPath = trimString(_request.getMyserver()->getRoot() + "/" + tempStr, "/", START);
			int check = isGoodPath(finalPath);
			if (check == true)
			{
				_request.setPath(finalPath);
				return (true);
			}
			else if (check == 0)
				status = 0;
			else if (check == -1 && status != 0)
				status = -1;
		}
		if (status == -1)
			server_log(std::string(REDD) + "Request fd " + ss.str() + " cannot find any valide path root/index", ERROR);
		if (status == 0)
			server_log(std::string(REDD) + "Request fd " + ss.str() + " cannot access any path root/index", ERROR);
	}
	else
	{
		tempStr = trimString(_request.getPath(), "/", START);
		finalPath = trimString(_request.getMyserver()->getRoot() + "/" + tempStr, "/", START);
		status = isGoodPath(finalPath);
		if (status == true)
		{
			_request.setPath(finalPath);
			return (true);
		}
		else if (status == -1)
			server_log(std::string(REDD) + "Request fd " + ss.str() + " cannot find the path : " + finalPath, ERROR);
		else
			server_log(std::string(REDD) + "Request fd " + ss.str() + " cannot access path : " + finalPath, ERROR);
	}
	if (status == -1)
		_request.setStatusCode(404);
	if (status == 0)
		_request.setStatusCode(403);
	return (false);
}

int	HttpRequestError::isGoodPath(std::string &str)
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

bool    HttpRequestError::hasReadPermission(std::string &str)
{
	int status = access(str.c_str(), R_OK);
	if (status < 0)
		return (false);
	else
		return (true);
}

bool    HttpRequestError::hasWritePermission(std::string &str)
{
	int status = access(str.c_str(), W_OK);
	if (status < 0)
		return (false);
	else
		return (true);
}

bool    HttpRequestError::hasExecutePermission(std::string &str)
{
	int status = access(str.c_str(), X_OK);
	if (status < 0)
		return (false);
	else
		return (true);
}
