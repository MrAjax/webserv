#ifndef HTTPREQUESTALLOW_HPP
# define HTTPREQUESTALLOW_HPP

#include "../inc/webserv.hpp"

class HttpRequestAllow
{
	public:

		HttpRequestAllow(int const &maxRequests, double const &intervalUseconds);

		~HttpRequestAllow(void);
		//------------OPERATOR------------------//

        bool    allowRequest();

		private:
            struct timespec _lastResetTime;
            int             _requestCount;
            int             _maxRequests;
            double          _intervalUseconds;

};

#endif