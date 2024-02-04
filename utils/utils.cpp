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

std::string	build_header(int status_code, std::string content_type, size_t body_len) {
	server_log("Building header...", DEBUG);
	std::string	header("HTTP/1.1 ");
	
	header 	+= int_to_str(status_code) \
			+ " " 
			+ HttpStatusCode::get_error_msg(status_code) \
			+ "\r\n" \
			+ "Date: " + getTimestamp() \
			+ "\r\n" \
			+ "Server: Webserv" \
			+ "\r\n";
			// TODO --> Set the right date: Sat, 09 Oct 2010 14:28:02 GMT

	// if (body_len > 0) {
		header 	+= "Content-Type: " \
				+ content_type \
				+ "\r\n" \
				+ "Content-Length: " \
				+ int_to_str(body_len);
	// }
	header += "\r\n\r\n";
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
	return (str);
}

std::size_t	strToSize_t(std::string const &toConvert)
{
	if (toConvert.empty())
		throw std::runtime_error("ERROR: string to size_t convertor nothing to convert\n");
	if (toConvert[0] == '0' && toConvert.size() > 1)
		throw std::runtime_error("ERROR: string to size_t convertor bad synthax\n");
	for (std::size_t i = 0; i < toConvert.size(); i++)
	{
		if (!std::isdigit(toConvert[i]))
			throw std::runtime_error("ERROR: string to size_t convertor != digit\n");
	}
	std::stringstream ss;
	ss << toConvert;
	std::size_t	value;
	ss >> value;
	if (value == std::numeric_limits<std::size_t>::max())
		throw std::runtime_error("ERROR: string to size_t convertor value >= size_t MAX\n");
	return (value);
}