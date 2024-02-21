#ifndef METHOD_HPP
# define METHOD_HPP

#include "../inc/webserv.hpp"
#include "../utils/utils.hpp"
#include "../response/StatusSender.hpp"
#include "../cgi/Cgi.hpp"

class Method {
	std::string	_path;
	std::string	_raw_path;
	std::string	_root;
	std::string	_content_type;
	std::string	_connection_status;
	std::string	_body_request;
	std::string	_header;
	std::string	_body;
	int			_status_code;
public:
	Method(const std::string &path, const std::string &content, const std::string &connection_status);
	Method(const std::string &path, const std::string &raw_path, const std::string &root, const std::string &content, const std::string &connection_status, std::string &body_request);

	virtual ~Method();

	virtual void	execute_method(Server &serv) = 0;
	
	std::string		get_path();
	std::string		get_raw_path();
	std::string		get_root();
	std::string		get_content_type();
	std::string		get_connection_status();
	std::string		get_header();
	std::string		get_body();
	std::string		get_body_request();
	int				get_status_code();

	void			set_header(const std::string &header);
	void			set_body(const std::string &body);
	void			set_statuscode(const int &code);
	void			set_path(const std::string &path);
};

#endif