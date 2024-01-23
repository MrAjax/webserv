#include "HttpRequestParsing.hpp"


HttpRequestParsing::HttpRequestParsing(HttpRequest &request) : _request(request)
{
    
}

HttpRequestParsing::~HttpRequestParsing() {}

void    HttpRequestParsing::parsingHeader(void)
{
	if (_request.STATUS >= DONE_HEADER)
		return ;
	std::string delimiteur = "\r\n\r\n";
	std::size_t pos = _request.getSaveString().find(delimiteur);
	if (pos == std::string::npos)
	{
		_request.STATUS = PROCESSING_HEADER;
		return ;
	}
	_request.setHeaderRequest(_request.getSaveString().substr(0, pos));
	if (_request.getSaveString().size() > pos + delimiteur.size())
		_request.setSaveString(_request.getSaveString().substr(pos + delimiteur.size()));
	else
		_request.getSaveString() = "";
	parseAllAttributes(_request.getHeaderRequest());
    if (!_request.getStrContentLength().empty())
        _request.setContentLength(convert(_request.getStrContentLength()));
    _request.STATUS = DONE_HEADER;
}

void    HttpRequestParsing::parsingBody(void)
{
	if (_request.STATUS == DONE_ALL || _request.STATUS < DONE_HEADER)
		return ;
	if (!_request.getStrContentLength().empty()) //TODO attention plutot regarder la version size_t
	{
		_request.STATUS = DONE_ALL;
		return ;
	}
	if (_request.getSaveString().size() >= _request.getContentLength())
	{
		_request.setBodyRequest(_request.getSaveString().substr(0, _request.getContentLength()));
		_request.setSaveString("");
		_request.STATUS = DONE_ALL;
	}
	else
		_request.STATUS = PROCESSING_BODY;
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

std::size_t HttpRequestParsing::convert(std::string const &toConvert)
{
	if (toConvert.empty())
		throw std::runtime_error("ERROR: string to size_t convertor nothing to convert\n");
	if (toConvert[0] == '0' && toConvert.size() > 1)
		throw std::runtime_error("ERROR: string to size_t convertor bad synthax\n");
	for (std::size_t i = 0; i < toConvert.size(); i++)
	{
		if (!std::isdigit(toConvert[i]))
			throw std::runtime_error("ERROR: string to size_t convertor != digit\n");
	}
	std::stringstream ss;
	ss << toConvert;
	std::size_t	value;
	ss >> value;
	if (value == std::numeric_limits<std::size_t>::max())
		throw std::runtime_error("ERROR: string to size_t convertor value >= size_t MAX\n");
	return (value);
}

//-----------Header parser----------------

void	HttpRequestParsing::parsingHeader_method_path_http(std::string &line)
{
	std::stringstream	ss(line);
	std::string			output;
	ss >> output;
	if (output != "GET" && output != "POST" && output != "DELETE")
		throw std::runtime_error("ERROR: Cannot find method in request\n");
	else
		_request.setMethod(output);
	ss >> output;
	this->_request.setPath(output);
	ss >> output;
	this->_request.setHttp(output);
	if (ss >> output)
		throw std::runtime_error("ERROR: Bad request synthax in 1st line\n");
}

std::string	HttpRequestParsing::parsingHeader_rest(std::string &line, std::string const & keyWord)
{
	std::stringstream	ss(line);
	std::string			token;
	std::string			output;

	ss >> token;
	if (token != keyWord)
		return (output);
	else
	{
		while (ss >> token)
		{
			if (!output.empty())
				output += " ";
			output += token;
		}
	}
	return (output);
}

void    HttpRequestParsing::parseAllAttributes(std::string header)
{
	std::size_t nbParam = 13;
	std::size_t end_pos;
	std::string delimiteur = "\r\n";
	std::string	line;

	struct _tab tab[nbParam] = {{"Host:", &HttpRequest::getHost, &HttpRequest::setHost}, {"User-Agent:", &HttpRequest::getUserAgent, &HttpRequest::setUserAgent},
		{"Accept:", &HttpRequest::getAccept, &HttpRequest::setAccept}, {"Accept-Language:", &HttpRequest::getAcceptLanguage, &HttpRequest::setAcceptLanguage},
		{"Accept-Encoding:", &HttpRequest::getAcceptEncoding, &HttpRequest::setAcceptEncoding}, {"Connection:", &HttpRequest::getConnection, &HttpRequest::setConnection},
		{"Upgrade-Insecure-Requests:", &HttpRequest::getUpInsecureRqst, &HttpRequest::setUpInsecureRqst}, {"Referer:", &HttpRequest::getReferer, &HttpRequest::setReferer},
		{"Sec-Fetch-Dest:", &HttpRequest::getSecFetchDest, &HttpRequest::setSecFetchDest}, {"Sec-Fetch-Mode:", &HttpRequest::getSecFetchMode, &HttpRequest::setSecFetchMode},
		{"Sec-Fetch-Site:", &HttpRequest::getSecFetchSite, &HttpRequest::setSecFetchSite}, {"Content-Length:", &HttpRequest::getStrContentLength, &HttpRequest::setStrContentLength},
		{"Content-Type:", &HttpRequest::getContentType, &HttpRequest::setContentType}};
	
	end_pos = findLine(header, line, delimiteur);
	parsingHeader_method_path_http(line);
	while (end_pos != std::string::npos)
	{
		end_pos = findLine(header, line, delimiteur);
		for (std::size_t i = 0 ; i < nbParam; i++)
		{
			if (line.find(tab[i].key) != std::string::npos)
			{
				if (!((_request.*(tab[i].getter))().empty()))
					throw std::runtime_error("Error: Bad request synthax duplicate information");
				(_request.*(tab[i].setter))(parsingHeader_rest(line, tab[i].key));
				break ;
			}
		}
	}
}