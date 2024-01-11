#include "HttpStatusCode.hpp"

HttpStatusCode::HttpStatusCode() {
	int			code;
	size_t		n;
	std::string	status;
	
	_http_error_list = "101 Switching protocols\n102 Processing\n103 Early Hints\n200 OK\n201 Created\n202 Accepted\n203 Non-Authoritative Information\n204 No Content\n205 Reset Content\n206 Partial Content\n207 Multi-Status\n208 Already Reported\n226 IM Used\n300 Multiple Choices\n301 Moved Permanently\n302 Found (Previously \"Moved Temporarily\")\n303 See Other\n304 Not Modified\n305 Use Proxy\n306 Switch Proxy\n307 Temporary Redirect\n308 Permanent Redirect\n400 Bad Request\n401 Unauthorized\n402 Payment Required\n403 Forbidden\n404 Not Found\n405 Method Not Allowed\n406 Not Acceptable\n407 Proxy Authentication Required\n408 Request Timeout\n409 Conflict\n410 Gone\n411 Length Required\n412 Precondition Failed\n413 Payload Too Large\n414 URI Too Long\n415 Unsupported Media Type\n416 Range Not Satisfiable\n417 Expectation Failed\n418 I'm a Teapot\n421 Misdirected Request\n422 Unprocessable Entity\n423 Locked\n424 Failed Dependency\n425 Too Early\n426 Upgrade Required\n428 Precondition Required\n429 Too Many Requests\n431 Request Header Fields Too Large\n451 Unavailable For Legal Reasons\n500 Internal Server Error\n501 Not Implemented\n502 Bad Gateway\n503 Service Unavailable\n504 	Gateway Timeout\n505 HTTP Version Not Supported\n506 Variant Also Negotiates\n507 Insufficient Storage\n508 Loop Detected\n510 Not Extended\n511 Network Authentication Required";

	for (size_t i = 0; i < _http_error_list.length(); i++) {
		code = (_http_error_list[i] - '0') * 100 + (_http_error_list[i + 1] - '0') * 10 + (_http_error_list[i + 2] - '0');
		for (n = i + 4; n < _http_error_list.length(); n++) {
			if (_http_error_list[n] == '\n') {
				status = _http_error_list.substr(i + 4, n - i - 4);
				break;
			}
		}
		_http_status_code.insert(std::make_pair(code, status));
		while (_http_error_list[i] != '\n')
			i++;
	}
}

HttpStatusCode::~HttpStatusCode() {}

std::map<int, std::string>	&HttpStatusCode::get_http_status_code() {
		return (_http_status_code);
}
