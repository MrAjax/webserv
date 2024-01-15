/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 10:43:42 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/15 15:49:16 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "parsing.hpp"

class serv_config;
class Server {

typedef void (*FuncPtr)(std::string const&);

public:
	Server(std::vector<std::string> config, serv_config const& webserv, int i);
	~Server( void );
	
private:

	Server( void );

	void displayFunc(std::string const& line);
	void p_listen(std::string const& line);
	void p_host(std::string const& line);
	void p_server_name(std::string const& line);

	void configServer(void);

	serv_config const& _webserv;
	int	_i; // number of Server 0 is the first etc...
	struct addrinfo* _res;
	struct sockaddr_in _server_addr;
	std::string _node; // = ip or localhost
	std::string _service; // = port

};

#endif
