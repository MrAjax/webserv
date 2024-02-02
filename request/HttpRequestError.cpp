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
	// if (protocol() > 0)
	// 	return (4);
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
	// if (protocol() > 0)
	// 	return (4);
	return (0);   
}

int HttpRequestError::DELETE(void)
{
	if (_request.getPath().rfind("/monsite/", 0) == std::string::npos)
		return (2);
	int status = access(_request.getPath().c_str(), F_OK);
	if (status < 0)
		return (3);
	// if (protocol() > 0)
	// 	return (4);
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

int HttpRequestError::modifiePath(Server &server)
{
	std::string path;
	if (_request.getPath() == "/")
	{
		std::vector<std::string>::iterator it = server.getIndex().begin();
		while (it != server.getIndex().end())
		{
			path = server.getRoot() + *it;
			if (access(path.c_str(), F_OK) >= 0)
			{
				_request.setPath(path);
				return (1);
			}
			it++;
		}
		return (0);
	}
	else
	{
		path = server.getRoot() + _request.getPath();
		if (access(path.c_str(), F_OK) >= 0)
			return (1);
		return (0);
	}

}

// void	preparing(std::string &index)
// {
// 	if (index.size() == 0)
// 		return ;
// 	if (index.size() > 2 && index.rfind("./", 0) != std::string::npos)
// 		index = index.substr(1);
// 	else if (index[0] != '/')
// 		index = "/" + index;
// }

void	trimBeginStr(std::string &str, std::string const &toTrim)
{
	if (str.size() == 0 || toTrim.size() == 0 || str.size() < toTrim.size())
		return ;
    for (std::size_t i = 0; i < toTrim.size(); i++)
        if (str[i] != toTrim[i])
            return ;
    if (str.size() == toTrim.size())
        str = "";
    else
        str = str.substr(toTrim.size());
}

std::string HttpRequestError::getFinalPath(Server &server, std::string str)
{
	std::string finalPath;
	if (str == "/")
	{
		std::vector<std::string>::iterator it = server.getIndex().begin();
		while (it != server.getIndex().end())
		{
			str = *it;
			trimBeginStr(str, "/");
			finalPath = server.getRoot() + "/" + str;
            trimBeginStr(finalPath, "/");
			if (isGoodPath(finalPath))
				return (finalPath);
			it++;
		}
	}
	else
	{
		str = _request.getPath();
		trimBeginStr(str, "/");
		finalPath = server.getRoot() + "/" + str;
        trimBeginStr(finalPath, "/");
		if (isGoodPath(finalPath))
			return (finalPath);
	}
    std::stringstream ss;
	ss << _request.getConnfd();
	throw error_throw("Request fd " + ss.str() + " cannot access path : " + finalPath, true);
	return ("");
}

bool	HttpRequestError::isGoodPath(std::string &str)
{
	std::string tabMethod[3] = {"GET", "POST", "DELETE"};
	int choice = 0;
	for (;choice < 3; choice++)
		if (tabMethod[choice] == _request.getMethod())
			break ;
    std::stringstream ss;
	ss << _request.getConnfd();
	switch (choice)
	{
		case 0:
			return (hasReadPermission(str));
		case 1:
			return (hasWritePermission(str));
		case 2:
			return (hasReadPermission(str) && hasWritePermission(str) && hasExecutePermission(str));
		case 3:
				throw error_throw("Request fd " + ss.str() + " method not supported", false);
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
