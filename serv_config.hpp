/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_config.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 10:43:42 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/08 13:48:02 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVCONFIG_HPP
# define SERVCONFIG_HPP

#include "parsing.hpp"
#include "Server.hpp"

class serv_config {

public:
	serv_config(char* file);
	~serv_config( void );
	
private:
	serv_config( void );
	void parse_blocks( void );
	
	std::ifstream _ifs;
	int socketfd;
	std::vector<Server> _servers;
		
};

#endif
