/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 10:43:42 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/20 11:12:27 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "parsing.hpp"

class Server {

typedef void (*FuncPtr)(std::string const&);

public:
	Server(std::vector<std::string> config, std::vector<Server> const& servers, int i);
	~Server( void );

	std::string getIp( void ) const;
	std::string getPort( void ) const;
	std::string getServerName(void) const; 
	struct sockaddr_in getclientAddr( void ) const;
	int	getMaxBodySize(void) const;
	int	getSocketfd( void ) const;
	
private:

	Server( void );

	void displayFunc(std::string const& line);
	void p_listen(std::string const& line);
	void p_host(std::string const& line);
	void p_server_name(std::string const& line);
	void p_bodySize(std::string const& line);

	void configServer(void);
	void openSocket(void);

	int					_i; // number of Server 0 is the first etc...
	int				 	_socketfd;
	int					_max_body_size;
	std::vector<Server> _servers;
	struct addrinfo*	_res;
	struct sockaddr_in	_server_addr;
	std::string 		_ip;
	std::string 		_port;
	std::string			_server_name;
	bool				_socketIsSet;

};

#endif
