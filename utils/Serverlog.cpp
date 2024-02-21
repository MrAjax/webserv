#include "Serverlog.hpp"

std::fstream Serverlog::_log_file;

void    Serverlog::open_file() {
    if (!_log_file.is_open())
		_log_file.open(LOG_FILE, std::ios::out | std::ios::app);
	std::string		begin;

	if (!_log_file.is_open())
		error_throw("cannot open server log file", true);
}

void	Serverlog::server_log(std::string content, int log_level) {
	
    open_file();
    ///////////
    if (log_level > LOGLVL || !_log_file.is_open())
		return;
    
    if (!_log_file.is_open())
		_log_file.open(LOG_FILE, std::ios::out | std::ios::app);
	std::string		begin;

	if (!_log_file.is_open())
		error_throw("cannot open server log file", true);
    
	switch (log_level)
	{
	case ERROR:
		content = std::string(REDD) + getTimestamp() + " " + content + ENDD;
		break;
	case DIALOG:
		content = std::string(GREENN) + "\n" + content + ENDD;
		break;
	case INFO:
		content = std::string(BLUEE) + getTimestamp() + " " + content + ENDD;
		break;
	case DEBUG:
		content = std::string(YELLOWW) + "            " + content + ENDD;
		break;
	default:
		break;
	}
	_log_file << content << std::endl;
	_log_file.flush();
    /////////////////
    close_file();
}

void    Serverlog::close_file() {
    if (_log_file.is_open())
	    _log_file.close();
}