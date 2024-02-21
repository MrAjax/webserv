#include "HttpRequestBody.hpp"


HttpRequestBody::HttpRequestBody(HttpRequest &request) : _request(request), _contentLength(request.getContentLength()), _isChunked(false), _isABody(false)
{
    std::stringstream ss;
	ss << _request.getConnfd();
	_debugFd = ss.str();
	if (_request.getTransferEncoding() == "chunked")
		_isChunked = true;
	if (_contentLength > 0 || _isChunked == true)
		_isABody = true;
	_saveStringSize =_request.getSaveString().size();
}

HttpRequestBody::~HttpRequestBody() {}

bool	HttpRequestBody::isMaxSize(std::size_t SIZE)
{
	if (_request.getMyserver() != NULL && SIZE > _request.getMaxBodySize())
	{
		server_log("Request fd " + _debugFd + " Body > max_size " + int_to_str(SIZE) + " max size is " + int_to_str(_request.getMaxBodySize()), ERROR);
		return (_request.setStatusCode(413), true);
	}
	return (false);
}

int	HttpRequestBody::chunkProcessing()
{
	std::string copy = _request.getSaveString();
	std::string delimiteur = "\r\n";

	std::string	hexa1;
	std::size_t end_pos1 = findLine(copy, hexa1, delimiteur);
	
	std::string	data2;
	std::size_t end_pos2 = findLine(copy, data2, delimiteur);
	if (end_pos1 == std::string::npos || end_pos2 == std::string::npos)
		return -1;
	ssize_t size;
	std::stringstream(hexa1) >> std::hex >> size;
	if (size == 0)
		return (true);
	else if (size < 0)
	{
		server_log("Request fd " + _debugFd + " bad synthax in body: " + hexa1, ERROR);
		return (_request.setStatusCode(400), -1);
	}
	else if (size > static_cast<ssize_t>(data2.size()))
	{
		server_log("Request fd " + _debugFd + " bad synthax in body: body chunk size" + hexa1 + " > chunk data.size() " + int_to_str(data2.size()), ERROR);
		server_log("Request fd " + _debugFd + " data : " + data2, ERROR);
		return (_request.setStatusCode(400), -1);
	}
	_request.setBodyRequest(_request.getBodyRequest() + data2.substr(0, size));
	_request.setSaveString(copy);
	_request.setContentLength(_request.getContentLength() + size);
	return (false);
}

bool    HttpRequestBody::parsingBody(void)
{
	if (_isABody == false)
		return (_request.setStatusCode(202), true);
	if (_isChunked == true)
	{
		if (_request.getContentLength() == -1)
			_request.setContentLength(0);
		int status = false;
		for (;status == false;)
			status = chunkProcessing();
		if (status == true)
		{
			_request.setSaveString("");
			return (_request.setStatusCode(202), true);
		}
	}
	else
	{
		//server_log("Request fd " + _debugFd + " upload progress :	" + uploadPrint(_saveStringSize, _contentLength), INFO);
		if (isMaxSize(_saveStringSize) == true) {
			return (false);
		}
		if (_saveStringSize >= static_cast< std::size_t >(_contentLength))
		{
			_request.setBodyRequest(_request.getSaveString().substr(0, _contentLength));
			_request.setSaveString("");
			return (_request.setStatusCode(202), true);
		}
	}
	_request.setStatusCode(PROCESSING_BODY);
	return (false);
}
