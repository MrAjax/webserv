#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "HTTP.hpp"

class HttpResponse {
	std::string					_protocol;
	std::string					_status_code;
	std::string					_status_msg;
	std::string					_header;
	std::string					_response;
public:
	HttpResponse(HttpRequest &req);
//	HttpResponse(const HttpResponse &rep);
//	HttpResponse	&operator=(const HttpResponse &rep);
	~HttpResponse();

	int	getResponse();
};

// Building and storing the HTTP response (status line, header, body...)
// Setting the appropriate status code


/*

	PIERRE

	Ajouter des getters pour la classe HttpRequest
        std::string getMethod();
        std::string getPath();
        std::string getProtocol();
        std::string getHeader();
	
	Plus ajouter une fonction qui indique si la requete est valide
	Ou bien faire en sorte que la fn parsingHeader retourne un int en cas d'erreur
	
	Pas compris a quoi correspondent les attributs prives de la classe
	Notamment ContentType et ContentLenght -> On le voit commment dans la requete ?

*/

/*

	1- Locates the appropriate document and returns it.

*/

/*

	=== HTTP RESPONSE FORM ===

	HTTP/[VER] [CODE] [TEXT]
	Field1: Value1
	Field2: Value2

	...Document content here...

*/

#endif