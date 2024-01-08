/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bahommer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 12:33:52 by bahommer          #+#    #+#             */
/*   Updated: 2024/01/08 12:56:27 by bahommer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "parsing.hpp"

Server::Server(std::vector<std::string> config) {
	std::cout << "SERVER" << std::endl;
	for (size_t i = 0; i < config.size(); i++) {
		std::cout << config[i] << std::endl;
	}
	std::cout << "END OF SERVER" << std::endl;
}

Server::~Server(void) {}
