#include "utils.hpp"

std::string	int_to_str(int n) {
	std::stringstream	ss;

	ss << n;
	return ss.str();
}

std::string	getTimestamp(void)
{
	std::time_t t = std::time(0);
	std::tm		*ltm = std::localtime(&t);
	std::string	time_stamp;

	time_stamp = "[";
	if (ltm->tm_hour < 10)
		time_stamp += "0" + int_to_str(ltm->tm_hour) + ":";
	else
		time_stamp += int_to_str(ltm->tm_hour) + ":";
	if (ltm->tm_min < 10)
		time_stamp += "0" + int_to_str(ltm->tm_min) + ":";
	else
		time_stamp += int_to_str(ltm->tm_min) + ":";
	if (ltm->tm_sec < 10)
		time_stamp += "0" + int_to_str(ltm->tm_sec) + "] ";
	else
		time_stamp += int_to_str(ltm->tm_sec) + "] ";
	return time_stamp;
}

void	server_log(std::string content, int log_level) {
	if (log_level > LOGLVL)
		return ;

	std::fstream	log_file("server/log", std::ios::out | std::ios::app);
	std::string		begin;

	if (!log_file.is_open())
		error_throw("cannot open server log file");

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
	log_file << content << std::endl;
	log_file.close();
}

std::runtime_error	error_throw(std::string description) {
	std::string	error(std::string(REDD) + "ERROR: " \
						+ description \
						+ ": " \
						+ std::string(strerror(errno)) \
						+ std::string(ENDD));
	server_log(error, ERROR);
	return std::runtime_error(error);
}
