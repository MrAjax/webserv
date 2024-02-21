#ifndef HTTPREQUESTCHECKING_HPP
# define HTTPREQUESTCHECKING_HPP

#include "HttpRequest.hpp"

class HttpRequestChecking
{
	public:

		HttpRequestChecking(HttpRequest &request);

		~HttpRequestChecking(void);
		//------------OPERATOR------------------//

		//-----------PROTOCOL---------------
		bool        isGoodProtocol(std::string const &http);
		//-------------METHOD--------------
		int         GET();
		int         POST();
		int         DELETE();

		int         BuildAndCheckHeader();
		int         Path();
		//-------------FIND PATHS--------------
		Server      *findMyServer(std::vector<Server> &servers);
		bool 		findRootPath();
		bool 		findOtherPath();
		bool		findCgi();
		bool		setCgiPath();
		bool		setDownloadPath();
		//-------------FIND PATHS UTILS--------------
		int         checkSockfdPortIP(Server &servers);
		int			isGoodPath(std::string &str);
		int			checkPath(std::string const &path, Server const &serv, std::string &finalPath, bool addRoot);
		//-------------PERMISSION--------------
		bool		fileExiste(std::string &str);
		bool		hasReadPermission(std::string &str);
		bool		hasWritePermission(std::string &str);
		bool		hasExecutePermission(std::string &str);






		private:
			HttpRequest &_request;
			std::string _debugFd;



};

#endif