#include "HttpError.hpp"

// rediriger vers les pages d'erreur si besoin

HttpError::HttpError(int err) {(void)err;}

HttpError::~HttpError() {}

std::string	HttpError::get_response()	{return this->get_response();}
