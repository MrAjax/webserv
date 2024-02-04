#ifndef HTTPREQUESTCHECKING_HPP
# define HTTPREQUESTCHECKING_HPP

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <poll.h>
#include <sys/types.h>

#include <sys/socket.h>

#include "HttpRequest.hpp"

# define RESET	"\e[0m"
# define RED	"\e[31m"
# define GREEN	"\e[32m"
# define YELLOW	"\e[33m"
# define BLUE	"\e[34m"
# define PURPLE	"\e[35m"
# define CYAN	"\e[36m"

class HttpRequestChecking
{
	public:

		HttpRequestChecking(HttpRequest &request);

		~HttpRequestChecking(void);
		//------------OPERATOR------------------//

		//---------check error---------------------
		void        checkError();

		int         GET();
		int         POST();
		int         DELETE();
		bool        isGoodProtocol(std::string const &http);

		int         BuildAndCheckHeader();
		int         Path();


		Server      *findMyServer(std::vector<Server> &servers);

		int         checkSockfdPortIP(Server &servers);

		int        modifiePath(Server &server);

		bool		getFinalPath(void);

		bool 		findRootPath();
		bool 		findOtherPath();

		bool		findCgi();

		int			isGoodPath(std::string &str);

		bool       hasReadPermission(std::string &str);
		bool       hasWritePermission(std::string &str);
		bool       hasExecutePermission(std::string &str);

		private:
			HttpRequest &_request;
			std::string _debugFd;



};

#endif