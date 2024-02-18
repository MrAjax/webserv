#ifndef RESPONSESENDER_HPP
#define RESPONSESENDER_HPP

#include "../inc/webserv.hpp"
#include "../request/HttpRequest.hpp"
#include "../parsing/Server.hpp"
#include "HttpResponse.hpp"


#define MAXDATA_SEND 500000

class ResponseSender
{
	public:
		ResponseSender(HttpRequest &Req, struct pollfd &mypoll);
		~ResponseSender();

		bool		createChunk();
		void		putTransfertEncoding();
		bool		catchHeader();
		bool		isLastChunk();

		std::string chunk(std::string &str);

		void	send_first_response_to_client();
		int		send_response_to_client();
		void	send_response(int connfd, Server &serv ,HttpRequest &Req);

		void    closeRequest();

		void    processingChunk();

		// -----UTILS------
		std::size_t	sizeConvert(std::size_t strSize);
		bool		isMaxSize(std::size_t const &size);

	private:
		std::string 	_response;
		HttpRequest 	&_request;
		struct pollfd 	&_mypoll;
};

#endif