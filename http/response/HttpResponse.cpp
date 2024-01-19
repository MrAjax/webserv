#include "HttpResponse.hpp"

/* static	std::string	setContentType(std::string path) {
	std::string			content_type;
	std::ifstream		mime_file("httpconfig/MIME_types");
	std::stringstream	file_content;

	if (!mime_file.is_open())
		throw std::runtime_error("ERROR: Cannot open the MIME file\n");

	mime_file.close();

	content_type = "text/hmtl";

	return content_type;
} */

HttpResponse::HttpResponse(HttpRequest &req) {
	_method			= req.getMethod();
	_method_code	= _method.length();
	_path			= std::string(MYWEBSITE) + req.getPath();
//	_contentType	= setContentType(_path);
	_contentType	= "text/html";

	if (req.getPath() == "/" && _method_code == GET)
		_path = std::string(MYWEBSITE) + "/index.html";
	else if (_path == "/") {
		_status_code = 205;
		_status_msg = HttpStatusCode::get_error_msg(_status_code);
		_response = "HTTP/1.1 " + int_to_str(_status_code) \
				  + " " + _status_msg + "\r\n\r\n";
	}

	//FOR DEBUG TODO
	std::cout << "Constructor called for Response\n";
	std::cout << "method = " << _method << "\n";
	std::cout << "method code = " << _method_code << "\n";
	std::cout << "path = " << _path << "\n";
	std::cout << "content type = " << _contentType << "\n";
}

HttpResponse::~HttpResponse() {}

std::string	HttpResponse::get_response() {
	Method *m;

	switch (_method_code)
	{
	case GET:
		m = new Get(_path, _contentType); // _path a la place
		m->execute_method();
		break;
	case POST:
		m = new Post(_path, _contentType);
		break;
	case DELETE:
		m = new Delete(_path, _contentType);
		break;
	default:
		break;
	}

	_response = m->get_header() + m->get_body();
	// TODO --> set a MODE variable to print this output if needed
	std::cout << m->get_header() << "\n";
	delete m;
	return _response;
}
