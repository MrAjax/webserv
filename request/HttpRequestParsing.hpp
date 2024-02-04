#ifndef HTTPREQUESTPARSING_HPP
# define HTTPREQUESTPARSING_HPP

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <poll.h>
#include <sys/types.h>

#include <sys/socket.h>

#include <limits>
#include "HttpRequest.hpp"


class HttpRequestParsing
{
	public:

		HttpRequestParsing(HttpRequest &request);

		~HttpRequestParsing(void);
		//------------OPERATOR------------------//

		bool    	parsingHeader(void);
		void    	parsingBody(void);

		void        splitHeaderBody(std::string &saveString);
		bool        parsingHeader(std::string &header);
		bool        parsingHeader_method_path_http(std::string &line);
		bool	parsingHeader_rest(std::string &line, std::string const & keyWord, std::string & output);
        bool        parseAllAttributes(std::string header);

		//---------Utils---------------------
		std::size_t findLine(std::string &header, std::string &line, std::string &delimiteur);
		std::size_t convert(std::string const &toConvert);

        private:
            HttpRequest &_request;
			struct _tab
			{
    			const char* key;                  // La clé
    			std::string (HttpRequest::*getter)();  // Pointeur vers le getter
    			void (HttpRequest::*setter)(const std::string&);  // Pointeur vers le setter
			};
			std::string	_strFd;


};

#endif