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

        int         Method();

        int         Path();

        int         Other();

        private:
            HttpRequest &_request;



};

#endif