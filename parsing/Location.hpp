#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "../inc/webserv.hpp"

#define PARAM_LOC 2
typedef void (*FuncPtr)(std::string const&);

class Location {

public:

	Location(std::vector<std::string> config);
	~Location(void);

	std::string getRoot( void ) const;
	std::vector<std::string> getallow_methods( void ) const;

private:

	void	p_root(std::string const& line);
	void	p_allow_methods(std::string const& line);
	std::string _root;
	std::vector<std::string> _allow_methods;
};

#endif
