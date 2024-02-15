#ifndef RESPONSESENDER_HPP
#define RESPONSESENDER_HPP

#include "../inc/webserv.hpp"
#include "../request/HttpRequest.hpp"


#define SEND_MAX 500

class ResponseSender
{
	public:
		ResponseSender(std::string &response, HttpRequest &Req);
		~ResponseSender();

		int		chunked();
		bool	sendHeader();
		bool	isMaxSize();

	private:
		std::string _response;
		HttpRequest &_request;
};

#endif