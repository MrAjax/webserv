/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_config.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 10:43:42 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/16 11:51:00 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVCONFIG_HPP
# define SERVCONFIG_HPP

#include "parsing.hpp"

class Server;
class serv_config {

public:
	serv_config(char* file);
	~serv_config( void );
	
	int	getSocketfd( void ) const;
	std::vector<Server>& getServerList( void );

private:
	serv_config( void );
	void parse_blocks( void );
	void config_webserv(void);
	
	std::ifstream _ifs;
	int socketfd;
	std::vector<Server> _servers;
		
};

#endif
