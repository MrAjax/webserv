#include "Location.hpp"

Location::Location(std::vector<std::string> config) 
{
	(void) config;
	for (size_t i = 0; i < config.size(); i++) {
		std::cout << config[i] << std::endl;
	}	
}

Location::~Location(void) {}
