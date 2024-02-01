#include "Cgi.hpp"

Cgi::Cgi() {

}

Cgi::~Cgi() {_cgi_file.close();}

int	Cgi::exec_cgi(std::string path) {
// check si le fichier existe reellement dans le cgi-bin
// executer
	(void)path;
	return 0;
}
