/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 10:30:56 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/18 16:01:50 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HPP
# define PARSING_HPP

#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <exception>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <map>
#include <cstdlib>
#include <string>
#include <poll.h>
#include <map>

#include "Server.hpp"

# define PARAM 3
# define MAX_CO 10
class Server;

struct pollFdComparer {
    bool operator()(struct pollfd const* lhs, struct pollfd const* rhs) const {
        return lhs->fd < rhs->fd;
    }
};

/*readConfigFile.cpp*/
int readConfigFile (std::vector<Server> & servers, char const* file);

/*allocatePollFds.cpp*/
void	allocatePollFds(std::vector<Server> const& servers,
	std::vector<struct pollfd> & pollfds,
		std::map<struct pollfd*, Server const*, pollFdComparer> & fdsMap);
#endif
