#ifndef RESPONSESENDER_HPP
#define RESPONSESENDER_HPP

#include "../inc/webserv.hpp"
#include "../request/HttpRequest.hpp"
#include "HttpResponse.hpp"


#define SEND_MAX 500

class ResponseSender
{
	public:
		ResponseSender(HttpRequest &Req, struct pollfd &mypoll, Server &serv);
		~ResponseSender();

		bool	createChunk();
		void	putTransfertEncoding();
		bool	catchHeader();
		bool	isMaxSize(std::size_t const &size);
		bool	isLastChunk();

		void	send_first_response_to_client();
		int		send_response_to_client();
		void	send_response(int connfd, Server &serv ,HttpRequest &Req);

		void    closeRequest();

		void    processingChunk();

	private:
		std::string 	_response;
		HttpRequest 	&_request;
		struct pollfd 	&_mypoll;
};

#endif