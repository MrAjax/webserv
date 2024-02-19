#ifndef HTTPREQUESTHEADER_HPP
# define HTTPREQUESTHEADER_HPP

#include "HttpRequest.hpp"

#define MAX_HEADER_SIZE 1024

class HttpRequestHeader
{
	public:

		HttpRequestHeader(HttpRequest &request);

		~HttpRequestHeader(void);
		//------------OPERATOR------------------//

		bool    	parsingHeader(void);
		bool        parsingHeader_method_path_http(std::string &line);
		bool		parsingHeader_rest(std::string &line, std::string const & keyWord, std::string & output);
        bool        parseAllAttributes(std::string header);

		void		setHeaderBeginBody(std::size_t pos, std::size_t delimiteurSize);

		//---------Utils---------------------
		bool		isMaxSize(std::size_t SIZE);

        private:
            HttpRequest &_request;
			struct _tab
			{
    			const char* key;                  // La clé
    			std::string (HttpRequest::*getter)();  // Pointeur vers le getter
    			void (HttpRequest::*setter)(const std::string&);  // Pointeur vers le setter
			};
			std::string	_debugFd;


};

#endif