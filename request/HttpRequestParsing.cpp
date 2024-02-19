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
		server_log("Request fd " + _debugFd + " Header > max_size_header "  + int_to_str(SIZE), ERROR);
		_request.setStatusCode(431);
		return (true);
	}
	if (_request.getMyserver() != NULL && SIZE > _request.getMaxBodySize() &&
		(_request.getStatusCode() == DONE_HEADER_CHECKING || _request.getStatusCode() == PROCESSING_BODY))
	{
		server_log("Request fd " + _debugFd + " Body > max_size " + int_to_str(SIZE) + " max size is " + int_to_str(_request.getMaxBodySize()), ERROR);
		_request.setStatusCode(413);
		return (true);
	}
	return (false);
}

void	HttpRequestParsing::setHeaderBeginBody(std::size_t pos, std::size_t delimiteurSize)
{
	_request.setHeaderRequest(_request.getSaveString().substr(0, pos + delimiteurSize));

	_request.setSaveString(_request.getSaveString().substr(pos  + delimiteurSize));
	
	server_log("Request fd " + _debugFd + " pos delimiteur " + int_to_str(pos), ERROR);

	std::vector<unsigned char> temp = _request.getRecvLine();
	temp.erase(temp.begin(), temp.begin() + pos + delimiteurSize);
	server_log("Request clientFd " + _debugFd + " size temp after = " + int_to_str(temp.size()), INFO);
	_request.setNewBody(temp);
	_request.getRecvLine().clear();
	_request.getRecvLine() = temp;

	
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

int	HttpRequestParsing::chunked()
{
	std::string copy = _request.getSaveString();
	std::string delimiteur = "\r\n";

	std::string	hexa1;
	std::size_t end_pos1 = findLine(copy, hexa1, delimiteur);
	
	std::string	data2;
	std::size_t end_pos2 = findLine(copy, data2, delimiteur);
	// server_log("Request fd " + _debugFd + " hexa [" + hexa1 + "] data [" + data2 + "]", ERROR);
	if (end_pos1 == std::string::npos || end_pos2 == std::string::npos)
		return -1;
	ssize_t size;
	std::stringstream(hexa1) >> std::hex >> size;
	if (size == 0)
		return (true);
	else if (size < 0)
	{
		server_log("Request fd " + _debugFd + " bad synthax in body: " + hexa1, ERROR);
		_request.setStatusCode(400);
		return (-1);
	}
	else if (size > static_cast<ssize_t>(data2.size()))
	{
		server_log("Request fd " + _debugFd + " bad synthax in body: body chunk size" + hexa1 + " > chunk data.size() " + int_to_str(data2.size()), ERROR);
		server_log("Request fd " + _debugFd + " data : " + data2, ERROR);
		_request.setStatusCode(400);
		return (-1);
	}
	_request.setBodyRequest(_request.getBodyRequest() + data2.substr(0, size));
	_request.setSaveString(copy);
	_request.setContentLength(_request.getContentLength() + size);
	return (false);
}

bool    HttpRequestParsing::parsingBody(void)
{
	if (_request.getContentLength() <= 0 && _request.getTransferEncoding().empty())
	{
		_request.setStatusCode(202);
		return (true);
	}

	
	if (_request.getTransferEncoding() == "chunked")
	{
		if (_request.getContentLength() == -1)
			_request.setContentLength(0);
		int status = false;
		for (;status == false;)
			status = chunked();
		if (status == true)
		{
			_request.setSaveString("");
			_request.setStatusCode(202);
			return (true);
		}
	}
	else
	{


		server_log("Request fd " + _debugFd + " head length " + int_to_str(_request.getHeaderRequest().size()), ERROR);
		server_log("Request fd " + _debugFd + " content-length BODY " + int_to_str(_request.getNewBody().size()), ERROR);
		server_log("Request fd " + _debugFd + " savestring length " + int_to_str(_request.getSaveString().size()), ERROR);
		server_log("Request clientFd " + _debugFd + " numbytes = " + int_to_str(_request.getNumBytes()), INFO);
		server_log("Request clientFd " + _debugFd + " recvline size = " + int_to_str(_request.getRecvLine().size()), INFO);
		if (isMaxSize(_request.getRecvLine().size()) == true) {
			return (false);
		}
		if (_request.getRecvLine().size() >= static_cast< std::size_t >(_request.getContentLength()))
		{

			std::vector<unsigned char> temp =_request.getRecvLine();
			temp.erase(temp.begin(), temp.begin() + _request.getContentLength());
			_request.setNewBody(temp);

			_request.setBodyRequest(_request.getSaveString().substr(0, _request.getContentLength()));
			server_log("Request fd " + _debugFd + " YOUHOU ", ERROR);

			_request.setSaveString("");
			_request.setStatusCode(202);
			return (true);
		}
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

bool	HttpRequestParsing::parsingHeader_rest(std::string &line, std::string const & keyWord, std::string &output)
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

bool    HttpRequestParsing::parseAllAttributes(std::string header)
{
	std::size_t nbParam = NBPARAM - 3;
	std::size_t end_pos;
	std::string delimiteur = "\r\n";
	std::string	line;

	struct _tab tab[nbParam] = {{"Host:", &HttpRequest::getHost, &HttpRequest::setHost}, {"User-Agent:", &HttpRequest::getUserAgent, &HttpRequest::setUserAgent},
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
		for (std::size_t i = 0 ; i < nbParam; i++)
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
