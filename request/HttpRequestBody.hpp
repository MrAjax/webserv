#ifndef HTTPREQUESTBODY_HPP
# define HTTPREQUESTBODY_HPP

#include "HttpRequest.hpp"

#define MAX_HEADER_SIZE 1024

class HttpRequestBody
{
	public:

		HttpRequestBody(HttpRequest &request);

		~HttpRequestBody(void);
		//------------OPERATOR------------------//

		bool    	parsingHeader(void);
		bool    	parsingBody(void);

		bool		isMaxSize(std::size_t SIZE);

		int			chunkProcessing();

		void		setHeaderBeginBody(std::size_t pos, std::size_t delimiteurSize);
		//---------Utils---------------------

        private:
            HttpRequest &_request;
			std::string	_debugFd;
			ssize_t		_contentLength;
			bool		_isChunked;
			bool		_isABody;
			std::size_t	_saveStringSize;

};

#endif