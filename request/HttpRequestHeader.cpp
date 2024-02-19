#include "HttpRequestHeader.hpp"


HttpRequestHeader::HttpRequestHeader(HttpRequest &request) : _request(request)
{
    std::stringstream ss;
	ss << _request.getConnfd();
	_debugFd = ss.str();
}

HttpRequestHeader::~HttpRequestHeader() {}

bool	HttpRequestHeader::isMaxSize(std::size_t SIZE)
{
	if (SIZE > MAX_HEADER_SIZE)
	{
		server_log("Request fd " + _debugFd + " Header > max_size_header "  + int_to_str(SIZE), ERROR);
		return (_request.setStatusCode(431), true);
	}
	return (false);
}

void	HttpRequestHeader::setHeaderBeginBody(std::size_t pos, std::size_t delimiteurSize)
{
	_request.setHeaderRequest(_request.getSaveString().substr(0, pos + delimiteurSize));

	_request.setSaveString(_request.getSaveString().substr(pos + delimiteurSize));
	
	server_log("Request fd " + _debugFd + " pos delimiteur " + int_to_str(pos), ERROR);

	std::vector<unsigned char> temp = _request.getRecvLine();
	temp.erase(temp.begin(), temp.begin() + pos + delimiteurSize);
	server_log("Request clientFd " + _debugFd + " size temp after = " + int_to_str(temp.size()), INFO);
	_request.setNewBody(temp);
	_request.getRecvLine().clear();
	_request.getRecvLine() = temp;
}

bool    HttpRequestHeader::parsingHeader(void)
{
	if (_request.getStatusCode() != NEW && _request.getStatusCode() != PROCESSING_HEADER)
		return (false);
	std::string delimiteur = "\r\n\r\n";
	std::size_t pos = _request.getSaveString().find(delimiteur);
	if (pos == std::string::npos)
	{
		if (isMaxSize(_request.getSaveString().size()) == true)
			return (false);
		_request.setStatusCode(PROCESSING_HEADER);
		return (false);
	}
	setHeaderBeginBody(pos, delimiteur.size());
	if (isMaxSize(_request.getHeaderRequest().size()) == true)
		return (false);
	if (parseAllAttributes(_request.getHeaderRequest()) == false)
		return (false);
    if (!_request.getStrContentLength().empty())
	{
		ssize_t temp = strToSsize_t(_request.getStrContentLength());
        if (temp == -1)
		{
			server_log("Request fd " + _debugFd + " bad synthax content-length : "
			+ _request.getStrContentLength(), ERROR);
			_request.setStatusCode(400);
			return (false);
		}
		_request.setContentLength(temp);
	}
    _request.setStatusCode(DONE_HEADER);
	return (true);
}

//-----------Header parser----------------

bool	HttpRequestHeader::parsingHeader_method_path_http(std::string &line)
{
	struct _tab 
	{                // La clé
    	void (HttpRequest::*setter)(const std::string&);
	};
	_tab tab[3] = {{&HttpRequest::setMethod}, {&HttpRequest::setPath}, {&HttpRequest::setHttp}};
	std::stringstream	ss(line);
	std::string			output;

	for (int i = 0; i < 3; i++)
	{
		if (!(ss >> output))
		{
			server_log("Request fd " + _debugFd + " bad synthax header in 1st line : " + line, ERROR);
			_request.setStatusCode(400);
			return (false);
		}
		(_request.*tab[i].setter)(output);
	}
	if (ss >> output)
	{
		server_log("Request fd " + _debugFd + " bad synthax header in 1st line : " + line, ERROR);
		_request.setStatusCode(400);
		return (false);
	}
	return (true);
}

bool	HttpRequestHeader::parsingHeader_rest(std::string &line, std::string const & keyWord, std::string &output)
{
	std::stringstream	ss(line);
	std::string			token;

	ss >> token;
	if (token != keyWord)
	{
		server_log("Request fd " + _debugFd + " bad synthax : " + line, ERROR);
		_request.setStatusCode(400);
		return (false);
	}
	else
	{
		while (ss >> token)
		{
			if (!output.empty())
				output += " ";
			output += token;
		}
	}
	return (true);
}

bool    HttpRequestHeader::parseAllAttributes(std::string header)
{
	std::size_t end_pos;
	std::string delimiteur = "\r\n";
	std::string	line;

	struct _tab tab[NBPARAMLESS3] = {{"Host:", &HttpRequest::getHost, &HttpRequest::setHost}, {"User-Agent:", &HttpRequest::getUserAgent, &HttpRequest::setUserAgent},
		{"Accept:", &HttpRequest::getAccept, &HttpRequest::setAccept}, {"Accept-Language:", &HttpRequest::getAcceptLanguage, &HttpRequest::setAcceptLanguage},
		{"Accept-Encoding:", &HttpRequest::getAcceptEncoding, &HttpRequest::setAcceptEncoding}, {"Connection:", &HttpRequest::getConnection, &HttpRequest::setConnection},
		{"Upgrade-Insecure-Requests:", &HttpRequest::getUpInsecureRqst, &HttpRequest::setUpInsecureRqst}, {"Referer:", &HttpRequest::getReferer, &HttpRequest::setReferer},
		{"Sec-Fetch-Dest:", &HttpRequest::getSecFetchDest, &HttpRequest::setSecFetchDest}, {"Sec-Fetch-Mode:", &HttpRequest::getSecFetchMode, &HttpRequest::setSecFetchMode},
		{"Sec-Fetch-Site:", &HttpRequest::getSecFetchSite, &HttpRequest::setSecFetchSite}, {"Content-Length:", &HttpRequest::getStrContentLength, &HttpRequest::setStrContentLength},
		{"Content-Type:", &HttpRequest::getContentType, &HttpRequest::setContentType}, {"Cookie:", &HttpRequest::getCookie, &HttpRequest::setCookie},
		{"Transfer-Encoding:", &HttpRequest::getTransferEncoding, &HttpRequest::setTransferEncoding}};
	
	end_pos = findLine(header, line, delimiteur);
	if (parsingHeader_method_path_http(line) == false)
		return (false);
	while (end_pos != std::string::npos)
	{
		end_pos = findLine(header, line, delimiteur);
		for (std::size_t i = 0 ; i < NBPARAMLESS3; i++)
		{
			if (line.find(tab[i].key) != std::string::npos)
			{
				if (!((_request.*(tab[i].getter))().empty()))
				{
					server_log("Request fd " + _debugFd + " bad synthax double info", ERROR);
					_request.setStatusCode(400);
					return (false);
				}
				std::string output;
				if (parsingHeader_rest(line, tab[i].key, output) == false)
					return (false);
				(_request.*(tab[i].setter))(output);
				break ;
			}
		}
	}
	_request.setBrutPath(_request.getPath());
	return (true);
}
