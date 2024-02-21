#include "HttpRequestChecking.hpp"

HttpRequestChecking::HttpRequestChecking(HttpRequest &request) : _request(request)
{
	std::stringstream ss;
	ss << _request.getConnfd();
	_debugFd = ss.str();
}

HttpRequestChecking::~HttpRequestChecking() {}

int HttpRequestChecking::GET(void)
{
	if (!isGoodProtocol("HTTP/1.1"))
		return (1);
	if (findCgi() == true && setCgiPath() == false)
	{
		server_log("Request fd " + _debugFd + " GET cannot find any valide cgi path", ERROR);
		return (_request.setStatusCode(404), 1);
	}
	else if (_request.getIsCgi())
		return (0);
	if (!findRootPath() && !findOtherPath())
		return (2);
	return (0);
}

int HttpRequestChecking::POST(void)
{
	if (!isGoodProtocol("HTTP/1.1"))
		return (3);
	
	if (!_request.getTransferEncoding().empty())
		server_log("Reqest fd " + _debugFd + " method POST transfert-encoding detected: " + _request.getTransferEncoding(), DEBUG);
	if (_request.getContentType().empty())
	{
		server_log("Reqest fd " + _debugFd + " method POST whitout content type", ERROR);
		return (_request.setStatusCode(411), 4);
	}
	if ((_request.getContentLength() != -1 && !_request.getTransferEncoding().empty())
		|| (_request.getContentLength() == -1 && _request.getTransferEncoding().empty()))
	{
		server_log("Reqest fd " + _debugFd + " method POST content lenght and transfer-encoding empty/full at the same time", ERROR);
		return (_request.setStatusCode(400), 5);
	}
	if (_request.getTransferEncoding() != "chunked" && _request.getContentLength() == -1)
	{
		server_log("Reqest fd " + _debugFd + " method POST bad synthax transfer-encoding: " + _request.getTransferEncoding(), ERROR);
		return (_request.setStatusCode(400), 5);
	}
	if (_request.getPath() == "/") //remove for accepting root POST
	{
		server_log("Reqest fd " + _debugFd + " method POST with path / not allow", ERROR);
		return (_request.setStatusCode(403), 6);
	}
	if (findCgi() == true && setCgiPath() == false)
	{
		server_log("Request fd " + _debugFd + " POST cannot find any valide cgi path", ERROR);
		return (_request.setStatusCode(404), 1);
	}
	else if (_request.getIsCgi())
		return (0);
	else
		setDownloadPath();
	return (0);   
}

int HttpRequestChecking::DELETE(void)
{
	if (!isGoodProtocol("HTTP/1.1"))
		return (8);
	
	if (_request.getPath() == "/") //remove for accepting root DELETE
	{
		server_log("Reqest fd " + _debugFd + " method DELETE with path / not allow", ERROR);
		_request.setStatusCode(403);
		return (9);
	}

	if (findCgi() == true && setCgiPath() == false)
	{
		server_log("Request fd " + _debugFd + " DELETE cannot find any valide cgi path", ERROR);
		return (_request.setStatusCode(404), 1);
	}
	else if (_request.getIsCgi())
		return (0);
	if (!findRootPath() && !findOtherPath())
		return (10);
	return (0);
}

int HttpRequestChecking::BuildAndCheckHeader(void)
{
	if (_request.getMyserver() == NULL)
		return (11);
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
		default :
			server_log("Request fd " + _debugFd + " method " + _request.getMethod() + " not allowed", ERROR);
			return (_request.setStatusCode(405), 12);
	}
	return (13);
}

bool HttpRequestChecking::isGoodProtocol(std::string const &http)
{
	if (_request.getHttp() == http)
		return (true);
	server_log("Request clientFd " + _debugFd + " protocol " + _request.getHttp() + " not allowed", ERROR);
	_request.setStatusCode(400);
	return (false);
}

int HttpRequestChecking::checkSockfdPortIP(Server &server)
{
	std::size_t pos = _request.getHost().find(":");
	if (pos == std::string::npos)
		return (-1);
	std::string requestIp = _request.getHost().substr(0, pos);
	if (requestIp == "127.0.0.1" || requestIp == "::1")
		requestIp = "localhost";
	std::string port = _request.getHost().substr(1 + pos);
	std::string serverIp = server.getIp();
	if (serverIp == "127.0.0.1" || serverIp == "::1")
		serverIp = "localhost";
	if (_request.getListenFd() == server.getSocketfd() && port == server.getPort()
		&& (requestIp == serverIp || requestIp == server.getServerName()))
		return (1);
	return (0);
}

Server  *HttpRequestChecking::findMyServer(std::vector<Server> &servers)
{
	Server  *findServer = NULL;

	std::size_t pos = _request.getHost().find(":");
	if (pos == std::string::npos)
	{
		server_log("Request clientFd " + _debugFd + " Host bad synthax : " + _request.getHost(), ERROR);
		_request.setStatusCode(400);
		return (NULL);
	}
	std::vector<Server>::reverse_iterator it = servers.rbegin();
	for (; it != servers.rend(); it++)
	{
		if (checkSockfdPortIP(*it))
			findServer = &(*it);
	}
	if (findServer == NULL)
	{
		_request.setStatusCode(400);
		server_log("Request fd " + _debugFd + " cannot find any server", ERROR);
	}
	return (findServer);
}

bool HttpRequestChecking::setDownloadPath()
{
	if (_request.getPath() == "/" && findRootPath() == false)
		return (false);

	Server *serv = _request.getMyserver();
	Location *loc = serv->getLocation("download");
	std::string finalPath;
	if (loc == NULL)
		checkPath("", *serv, finalPath, true);
	else
	{
		std::string root = loc->getRoot();
		// root = trimString(loc->getRoot(), "/", END);
		if (checkPath(root, *serv, finalPath, false) == -1)
		{
			server_log("Request clientFd " + _debugFd + " no download file on location : " + finalPath, ERROR);
			finalPath = trimString(serv->getRoot(), "/", START);
			server_log("Request clientFd " + _debugFd + " download file set on root site : " + finalPath, INFO);
		}
	}
	return (_request.setPath(finalPath), true);
}

bool HttpRequestChecking::setCgiPath()
{
	if (_request.getIsCgi() == false)
		return (false);
	Server *serv = _request.getMyserver();
	std::string processPath;
	if (checkPath(_request.getPath(), *serv, processPath, true) == true)
		return (_request.setPath(processPath), true);
	Location *loc = serv->getLocation("cgi-bin");
	if (loc == NULL || loc->getCgi_path().empty())
	{
		if (checkPath(_request.getPath(), *serv, processPath, true) == true)
			return (_request.setPath(processPath), true);
	}
	else
	{
		std::string endPath;
		std::size_t pos = _request.getPath().rfind("/");
		if (pos == std::string::npos)
			endPath = _request.getPath();
		else
			endPath = _request.getPath().substr(pos + 1);
		std::vector<std::string> temp = loc->getCgi_path();
		std::vector<std::string>::iterator it = temp.begin();
		for (;it != temp.end(); it++)
		{
			std::string str = trimString(*it, ".", START);
			processPath = trimString(str, "/", START) + "/" + endPath;
			std::string finalPath;
			if (checkPath(processPath, *serv, finalPath, true) == true)
				return (_request.setPath(finalPath), true);
		}
	}
	server_log("Request fd " + _debugFd + " no valid cgi-bin path has been found", ERROR);
	return (false);
}

bool HttpRequestChecking::findCgi()
{
	if (!is_cgi(_request.getPath()))
		return (false);
	_request.setIsCgi(true);
	_request.setPath(trimString(_request.getPath(), ".cgi", END));
	server_log("Request fd " + _debugFd + " find cgi", DEBUG);
	return (true);
}

//------------------ find PATHS -------------------------------

int HttpRequestChecking::checkPath(std::string const &path, Server const &serv, std::string &finalPath, bool addRoot)
{
	if (addRoot == true)
	{
		std::string tempStr = trimString(path, "/", START);
		finalPath = trimString(serv.getRoot() + "/" + tempStr, "/", START);
	}
	else
		finalPath = trimString(path, "/", START);
	return (isGoodPath(finalPath));
}

bool HttpRequestChecking::findRootPath()
{
	if (_request.getPath() != "/" || _request.getMyserver() == NULL)
		return (false);
	int status = -1;
	Server *serv = _request.getMyserver();
	std::string finalPath;
	std::vector<std::string> temp = _request.getMyserver()->getIndex();
	std::vector<std::string>::iterator it = temp.begin();
	for (;it != temp.end(); it++)
	{
		int check = checkPath(*it, *serv, finalPath, true);
		if (check == true)
			return (_request.setPath(finalPath), true);
		else if (check == false)
			status = false;
	}
	switch (status)
	{
		case -2:
			server_log("Request fd " + _debugFd + " method " + _request.getMethod() + " not allowed", ERROR);
			return (_request.setStatusCode(405), false);
		case -1:
			server_log("Request fd " + _debugFd + " cannot find any valide path root/index", ERROR);
			return (_request.setStatusCode(404), false);
		case false:
			server_log("Request fd " + _debugFd + " cannot access any path root/index", ERROR);
			return (_request.setStatusCode(403), false);
		default :
			return (false);
	}
	return (false);
}

bool HttpRequestChecking::findOtherPath()
{
	if (_request.getPath() == "/" || _request.getMyserver() == NULL)
		return (false);
	Server *serv = _request.getMyserver();
	std::string finalPath;
	int status = checkPath(_request.getPath(), *serv, finalPath, true);
	switch (status)
	{
		case true:
			return (_request.setPath(finalPath), true);
		case false:
			server_log("Request fd " + _debugFd + " cannot access path : " + finalPath, ERROR);
			return (_request.setStatusCode(403), false);
		case -1:
			server_log("Request fd " + _debugFd + " cannot find the path : " + finalPath, ERROR);
			return (_request.setStatusCode(404), false);
		case -2:
			server_log("Request fd " + _debugFd + " method " + _request.getMethod() + " not allowed", ERROR);
			return (_request.setStatusCode(405), false);
		default :
			return (false);
	}
	return (false);
}

//----------------Checking PATHS ----------------------

int	HttpRequestChecking::isGoodPath(std::string &str)
{
	if (fileExiste(str) == false)
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
		default :
			return (-2);
	}
	return (false);
}

bool    HttpRequestChecking::fileExiste(std::string &str)
{
	int status = access(str.c_str(), F_OK);
	if (status < 0)
		return (false);
	else
		return (true);
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
