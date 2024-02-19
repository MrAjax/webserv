#ifndef HTTPREQUESTPARSING_HPP
# define HTTPREQUESTPARSING_HPP

#include "HttpRequest.hpp"

#define MAX_HEADER_SIZE 1024

class HttpRequestParsing
{
	public:

		HttpRequestParsing(HttpRequest &request);

		~HttpRequestParsing(void);
		//------------OPERATOR------------------//

		bool    	parsingHeader(void);
		bool    	parsingBody(void);

		void        splitHeaderBody(std::string &saveString);
		bool        parsingHeader(std::string &header);
		bool        parsingHeader_method_path_http(std::string &line);
		bool		parsingHeader_rest(std::string &line, std::string const & keyWord, std::string & output);
        bool        parseAllAttributes(std::string header);

		bool		isMaxSize(std::size_t SIZE);

		int			chunked();

		void		setHeaderBeginBody(std::size_t pos, std::size_t delimiteurSize);
		//---------Utils---------------------
		std::size_t findLine(std::string &header, std::string &line, std::string &delimiteur);

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