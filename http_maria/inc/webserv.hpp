#ifndef WEBSERV_HPP
# define WEBSERV_HPP
// C libraries
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/time.h>
#include <signal.h>
#include <dirent.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <stdarg.h>
//C++ libraries
#include <sstream>
#include <cstring>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <ctime>
// General
#define SA struct sockaddr
// Server config
#define SERVER_PORT 18000
#define MAXLINE 4096
#define MYWEBSITE "Zzewebsite"
// Log file config
enum {NOLOG, ERROR, DIALOG, INFO, DEBUG};
#define LOGLVL DEBUG
#define GREENN "\033[32;1m"
#define YELLOWW "\033[38;5;227;1m"
#define REDD "\033[31;1m"
#define BLUEE "\033[36;1m"
#define WHITEE "\033[37;1m"
#define ENDD "\033[0m"
// End
#endif