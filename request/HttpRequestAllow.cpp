#include "HttpRequestAllow.hpp"

HttpRequestAllow::HttpRequestAllow(int const &maxRequests, double const &intervalUseconds) : _requestCount(0), _maxRequests(maxRequests), _intervalUseconds(intervalUseconds)
{
	clock_gettime(CLOCK_REALTIME, &_lastResetTime);
}

HttpRequestAllow::~HttpRequestAllow(void)	{/* std::cout << BLUE << _connfd << " Destructor call\n" << RESET; */}


bool HttpRequestAllow::allowRequest()
{
		struct timespec currentTime;
		clock_gettime(CLOCK_REALTIME, &currentTime);
        double currentInterval = ((currentTime.tv_sec - _lastResetTime.tv_sec) * 1e9 + (currentTime.tv_nsec - _lastResetTime.tv_nsec)) / 1000;
        if (currentInterval >= _intervalUseconds)
		{
            _requestCount = 0; // reset count et lastTime si on a des req suffisament espacés
            _lastResetTime = currentTime;
        }

        // Vérifier si le nombre de requêtes dépasse la limite
        if (_requestCount < _maxRequests)
		{
            // Autoriser la requête et incrémenter le compteur
            _requestCount++;
            return true;
        }
		else
            return false;
}