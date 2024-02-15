#ifndef RESPONSESENDER_HPP
#define RESPONSESENDER_HPP

#include "../inc/webserv.hpp"
#include "../request/HttpRequest.hpp"


#define SEND_MAX 500

class ResponseSender
{
	public:
		ResponseSender(std::string &response, HttpRequest &Req, struct pollfd &mypoll);
		~ResponseSender();

		bool		createChunk();
		int		putTransfertEncoding();
		bool	catchHeader();
		bool	isMaxSize(std::string const &str);
		bool	isLastChunk();

		int		send_response_to_client();
		void	send_response(int connfd, Server &serv ,HttpRequest &Req);

		void    closeRequest();

	private:
		std::string 	_response;
		HttpRequest 	&_request;
		struct pollfd 	&_mypoll;
};

#endif