#ifndef HTTPREQUESTERROR_HPP
# define HTTPREQUESTERROR_HPP

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

# define RESET	"\e[0m"
# define RED	"\e[31m"
# define GREEN	"\e[32m"
# define YELLOW	"\e[33m"
# define BLUE	"\e[34m"
# define PURPLE	"\e[35m"
# define CYAN	"\e[36m"

class HttpRequestError
{
	public:

		HttpRequestError(HttpRequest &request);

		~HttpRequestError(void);
		//------------OPERATOR------------------//

		//---------check error---------------------
		void        checkError();

		int         GET();
		int         POST();
		int         DELETE();
		bool        isGoodProtocol();

		int         Method();
		int         Path();
		int         maxSize();

		int         socketfdServers(std::vector<Server> &servers);

		Server      *findMyServer(std::vector<Server> &servers);

		int         checkPortIP(Server &servers);

		int        modifiePath(Server &server);

		std::string	getFinalPath(Server &server, std::string str);



		bool		isGoodPath(std::string &str);

		bool       hasReadPermission(std::string &str);
		bool       hasWritePermission(std::string &str);
		bool       hasExecutePermission(std::string &str);

		private:
			HttpRequest &_request;



};

#endif