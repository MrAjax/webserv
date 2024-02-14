#ifndef RESPONSESENDER_HPP
#define RESPONSESENDER_HPP

#include "../inc/webserv.hpp"

#define SEND_MAX 500

class ResponseSender
{
	public:
		ResponseSender(std::string &response);
		~ResponseSender();

		int		chunked();
		bool	isMaxSize();

	private:
		std::string _response;
};

#endif