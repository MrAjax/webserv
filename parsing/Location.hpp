#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "../inc/webserv.hpp"

#define PARAM_LOC 7
typedef void (*FuncPtr)(std::string const&);

class Location {

public:

	Location( std::vector<std::string> config );
	Location( Location const& a);
	Location& operator=( Location const& a);
	~Location(void);

	std::string getRoot( void ) const;
	std::string getReturn( void ) const;
	std::string getIndex( void ) const;
	std::vector<std::string> getallow_methods( void ) const;
	std::vector<std::string> getCgi_path( void ) const;
	std::vector<std::string> getCgi_ext( void ) const;
	bool	getAutoindex( void ) const;

private:

	void	p_root(std::string const& line);
	void	p_allow_methods(std::string const& line);
	void	p_httpRedirection(std::string const& line);
	void	p_autoindex(std::string const& line);
	void	p_index(std::string const& line);
	void	p_cgi_path(std::string const& line);
	void	p_cgi_ext(std::string const& line);

	std::string _root;
	std::string _return;
	std::string _index;
	std::vector<std::string> _allow_methods;
	bool	_autoindex;
	std::vector<std::string> _cgi_ext;
	std::vector<std::string> _cgi_path;
};

#endif
