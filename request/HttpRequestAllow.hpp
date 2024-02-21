#ifndef HTTPREQUESTALLOW_HPP
# define HTTPREQUESTALLOW_HPP

#include "../inc/webserv.hpp"
#include "HttpRequest.hpp"


#define MAX_NUMBER_REQUEST 1000

class HttpRequestAllow
{
	public:

		HttpRequestAllow(int const &maxRequests, double const &intervalUseconds);

		~HttpRequestAllow(void);
		//------------OPERATOR------------------//

        bool    allowRequest(std::vector<struct pollfd> &pollfds, HttpRequest &Request);

		private:
            struct timespec _lastResetTime;
            int             _requestCount;
            int             _maxRequests;
            double          _intervalUseconds;

};

#endif