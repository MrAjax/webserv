/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 10:43:42 by bahommer          #+#    #+#             */
/*   Updated: 2024/02/02 11:32:12 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "../inc/parsing.hpp"
#include "Location.hpp"

#define IPV4 4
#define IPV6 6


class Server {

typedef void (*FuncPtr)(std::string const&);

public:

	Server( std::vector<std::string> config, std::vector<Server> const& servers, int i );
	Server& operator = ( Server const& a);
	Server( Server const& a);
	~Server( void );

	std::string getIp( void ) const;
	std::string getPort( void ) const;
	std::string getServerName( void ) const; 
	std::string getRoot( void ) const;
	std::string getLocationErrorPage( void ) const;
	std::vector<std::string> getIndex( void )const;
//	struct sockaddr_in getclientAddr( void ) const;
	int	getMaxBodySize( void ) const;
	int	getSocketfd( void ) const;
	std::vector<int> getErrorPages( void ) const;
	Location *getLocation( std::string type ) const;
	
private:

	Server( void );

	void displayFunc(std::string const& line);
	void p_listen(std::string const& line);
	void p_host(std::string const& line);
	void p_server_name(std::string const& line);
	void p_bodySize(std::string const& line);
	void p_root(std::string const& line);
	void p_errorPage(std::string const& line);
	void p_index(std::string const& line);

	void configServer(void);
	void openSocket(void);

	std::string settempLocation(std::string line);

	int					_i; // number of Server 0 is the first etc...
	int				 	_socketfd;
	int					_max_body_size;
	int					_ipv_type;
	std::vector<int>	_error_pages;
	std::vector<Server> _servers;
	struct addrinfo*	_res;
//	struct sockaddr_in	_server_addr_ipv4;
//	struct sockaddr_in6	_server_addr_ipv6;	
	std::string 		_ip;
	std::string 		_port;
	std::string			_server_name;
	std::string			_root;
	std::string			_location_error_page;
	std::vector<std::string> _index;	
	bool				_socketIsSet;
	std::map<std::string, Location*> _locations;

};

#endif
