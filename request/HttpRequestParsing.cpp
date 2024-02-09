#include "HttpRequestParsing.hpp"


HttpRequestParsing::HttpRequestParsing(HttpRequest &request) : _request(request)
{
    std::stringstream ss;
	ss << _request.getConnfd();
	_debugFd = ss.str();
}

HttpRequestParsing::~HttpRequestParsing() {}

bool	HttpRequestParsing::isMaxSize(std::size_t SIZE)
{
	if (SIZE > MAX_HEADER_SIZE &&
		(_request.getStatusCode() == PROCESSING_HEADER || _request.getStatusCode() == NEW))
	{
		server_log(std::string(REDD) + "Request fd " + _debugFd + " Header > max_size_header", ERROR);
		_request.setStatusCode(431);
		return (true);
	}
	if (_request.getMyserver() != NULL && SIZE > _request.getMaxBodySize() &&
		(_request.getStatusCode() == DONE_HEADER_CHECKING || _request.getStatusCode() == PROCESSING_BODY))
	{
		server_log(std::string(REDD) + "Request fd " + _debugFd + " Body > max_size", ERROR);
		_request.setStatusCode(413);
		return (true);
	}
	return (false);
}

bool    HttpRequestParsing::parsingHeader(void)
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
	_request.setHeaderRequest(_request.getSaveString().substr(0, pos));
	if (_request.getSaveString().size() > pos + delimiteur.size())
		_request.setSaveString(_request.getSaveString().substr(pos + delimiteur.size()));
	else
		_request.getSaveString() = "";
	if (isMaxSize(_request.getHeaderRequest().size()) == true)
		return (false);
	if (parseAllAttributes(_request.getHeaderRequest()) == false)
		return (false);
    if (!_request.getStrContentLength().empty())
	{
		try{
        	_request.setContentLength(strToSize_t(_request.getStrContentLength()));
		} catch (std::exception &e) {
			server_log(std::string(REDD) + "Request fd " + _debugFd + " bad synthax content-length : "
			+ _request.getStrContentLength(), ERROR);
			_request.setStatusCode(400);
			return (false);
		}
	}
    _request.setStatusCode(DONE_HEADER);
	return (true);
}

bool    HttpRequestParsing::parsingBody(void)
{
	if (_request.getStatusCode() != DONE_HEADER_CHECKING && _request.getStatusCode() != PROCESSING_BODY)
		return (false);
	if (_request.getContentLength() == 0)
	{
		_request.setStatusCode(202);
		return (true);
	}
	if (isMaxSize(_request.getSaveString().size()) == true)
		return (false);
	if (_request.getSaveString().size() >= _request.getContentLength())
	{
		_request.setBodyRequest(_request.getSaveString().substr(0, _request.getContentLength()));
		_request.setSaveString("");
		_request.setStatusCode(202);
		return (true);
	}
	_request.setStatusCode(PROCESSING_BODY);
	return (false);
}

//-----------Utils----------------

std::size_t	HttpRequestParsing::findLine(std::string &header, std::string &line, std::string &delimiteur)
{
	std::size_t	end_pos = header.find(delimiteur);
	if (end_pos == std::string::npos)
	{
		line = header;
		return (end_pos);
	}
	line = header.substr(0, end_pos);
	header = header.substr(end_pos + delimiteur.size());
	return (end_pos);
}

//-----------Header parser----------------

bool	HttpRequestParsing::parsingHeader_method_path_http(std::string &line)
{
	std::stringstream	ss(line);
	std::string			output;
	ss >> output;
	_request.setMethod(output);
	ss >> output;
	this->_request.setPath(output);
	ss >> output;
	this->_request.setHttp(output);
	if (ss >> output)
	{
		server_log(std::string(REDD) + "Request fd " + _debugFd
		+ " bad synthax : " + line, ERROR);
		_request.setStatusCode(400);
		return (false);
	}
	return (true);
}

bool	HttpRequestParsing::parsingHeader_rest(std::string &line, std::string const & keyWord, std::string &output)
{
	std::stringstream	ss(line);
	std::string			token;

	ss >> token;
	if (token != keyWord)
	{
		server_log(std::string(REDD) + "Request fd " + _debugFd + " bad synthax : " + line, ERROR);
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

bool    HttpRequestParsing::parseAllAttributes(std::string header)
{
	std::size_t nbParam = 14;
	std::size_t end_pos;
	std::string delimiteur = "\r\n";
	std::string	line;

	struct _tab tab[NBPARAM] = {{"Host:", &HttpRequest::getHost, &HttpRequest::setHost}, {"User-Agent:", &HttpRequest::getUserAgent, &HttpRequest::setUserAgent},
		{"Accept:", &HttpRequest::getAccept, &HttpRequest::setAccept}, {"Accept-Language:", &HttpRequest::getAcceptLanguage, &HttpRequest::setAcceptLanguage},
		{"Accept-Encoding:", &HttpRequest::getAcceptEncoding, &HttpRequest::setAcceptEncoding}, {"Connection:", &HttpRequest::getConnection, &HttpRequest::setConnection},
		{"Upgrade-Insecure-Requests:", &HttpRequest::getUpInsecureRqst, &HttpRequest::setUpInsecureRqst}, {"Referer:", &HttpRequest::getReferer, &HttpRequest::setReferer},
		{"Sec-Fetch-Dest:", &HttpRequest::getSecFetchDest, &HttpRequest::setSecFetchDest}, {"Sec-Fetch-Mode:", &HttpRequest::getSecFetchMode, &HttpRequest::setSecFetchMode},
		{"Sec-Fetch-Site:", &HttpRequest::getSecFetchSite, &HttpRequest::setSecFetchSite}, {"Content-Length:", &HttpRequest::getStrContentLength, &HttpRequest::setStrContentLength},
		{"Content-Type:", &HttpRequest::getContentType, &HttpRequest::setContentType}, {"Cookie:", &HttpRequest::getCookie, &HttpRequest::setCookie}};
	
	end_pos = findLine(header, line, delimiteur);
	if (parsingHeader_method_path_http(line) == false)
		return (false);
	while (end_pos != std::string::npos)
	{
		end_pos = findLine(header, line, delimiteur);
		for (std::size_t i = 0 ; i < nbParam; i++)
		{
			if (line.find(tab[i].key) != std::string::npos)
			{
				if (!((_request.*(tab[i].getter))().empty()))
				{
					server_log(std::string(REDD) + "Request fd " + _debugFd
					+ " bad synthax double info", ERROR);
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
	return (true);
}
