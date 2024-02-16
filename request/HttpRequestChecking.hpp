#ifndef HTTPREQUESTCHECKING_HPP
# define HTTPREQUESTCHECKING_HPP

#include "HttpRequest.hpp"

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
		void		cgiPath();

		int			isGoodPath(std::string &str);

		bool       hasReadPermission(std::string &str);
		bool       hasWritePermission(std::string &str);
		bool       hasExecutePermission(std::string &str);

		private:
			HttpRequest &_request;
			std::string _debugFd;



};

#endif