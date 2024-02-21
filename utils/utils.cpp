#include "utils.hpp"

std::string	int_to_str(long int n) {
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

	std::fstream	log_file(LOG_FILE, std::ios::out | std::ios::app);
	std::string		begin;

	if (!log_file.is_open())
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
	log_file << content << std::endl;
	log_file.close();
}

std::runtime_error	error_throw(std::string description, bool errno_need) {
	std::string	error(std::string(REDD) + "ERROR: " \
						+ description);
	if (errno_need == true) {
		error += ": " + std::string(strerror(errno));
	}	
	error += std::string(ENDD);
	server_log(error, ERROR);
	return std::runtime_error(error);
}

std::string	build_header(int status_code, std::string content_type, size_t body_len, std::string connection_status) {
	server_log("Building header...", DEBUG);
	std::string	header("HTTP/1.1 ");

	header 	+= int_to_str(status_code) + " " + HttpStatusCode::get_error_msg(status_code) \
			+ "\r\nDate: " + getTimestamp() \
			+ "\r\nServer: Webserv" \
			+ "\r\nContent-Type: " + content_type \
			+ "\r\nContent-Length: " + int_to_str(body_len) \
			+ "\r\nConnection: " + connection_status + "\r\n\r\n";
	server_log("Header ready", DEBUG);
	return (header);
}

std::string basic_page(std::string title, std::string p) {
	std::string	page;

	page =  "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "<head>\n"
            "    <meta charset=\"UTF-8\">\n"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
            "    <title>" + title + "</title>\n"
            "</head>\n"
            "<body>\n"
            "    <header>\n"
            "        <h1>" + title + "</h1>\n"
			"        <p>" + p + "</p>\n"
            "    </header>\n"
            "</body>\n"
            "</html>\n";
	return page;
}

bool	is_cgi(std::string file_name) {
	std::string	type;

	type = ContentTypeFinder::get_content_type(file_name);
	if (type != "cgi")
		return (false);
	return (true);
}

std::string	trimString(std::string str, std::string const &toTrim, int STARTorEND)
{
	if (str.size() == 0 || toTrim.size() == 0 || str.size() < toTrim.size())
		return (str);
	if (str == toTrim)
		return ("");
	if (STARTorEND == START){
		for (std::size_t i = 0; i < toTrim.size(); i++)
			if (str[i] != toTrim[i])
				return (str);
		return (str.substr(toTrim.size()));
	}
	if (STARTorEND == END){
		std::size_t lenStr = str.size();
		lenStr--;
		std::size_t lenToTrim = toTrim.size();
		lenToTrim--;
		for (std::size_t i = 0; i < toTrim.size(); i++)
			if (str[lenStr--] != toTrim[lenToTrim--])
				return (str);
		return (str.substr(0, (str.size() - toTrim.size())));
	}
	if (STARTorEND == ALL){
		std::string doubleTrim = trimString(str, toTrim, START);
		doubleTrim = trimString(doubleTrim, toTrim, END);
		return (doubleTrim);
	}
	return (str);
}

ssize_t	strToSsize_t(std::string const &toConvert)
{
	if (toConvert.empty())
		return (-1);
	if (toConvert[0] == '0' && toConvert.size() > 1)
		return (-1);
	for (std::size_t i = 0; i < toConvert.size(); i++)
	{
		if (!std::isdigit(toConvert[i]))
			return (-1);
	}
	std::stringstream ss;
	ss << toConvert;
	ssize_t	value;
	ss >> value;
	if (value == std::numeric_limits<ssize_t>::max())
	{
		return (-1);
	}
	return (value);
}

std::size_t	findLine(std::string &header, std::string &line, std::string &delimiteur)
{
	std::size_t	end_pos = header.find(delimiteur);
	if (end_pos == std::string::npos)
	{
		line = header;
		return (end_pos);
	}
	line = header.substr(0, end_pos);
	header = header.substr(end_pos + delimiteur.size());
	return (end_pos);
}