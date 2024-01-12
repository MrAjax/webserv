#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>

#include <fstream>

#define SERVER_PORT 18000

#define MAXLINE 4096
#define SA struct sockaddr

//affiche les potentiels erreurs puis meurt
void	err_n_die(const char *fmt, ...) {
	int		errno_save;
	va_list	ap;

	errno_save = errno;

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	fprintf(stdout, "\n");
	fflush(stdout);

	if (errno_save != 0) {
		fprintf(stdout, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
		fprintf(stdout, "\n");
		fflush(stdout);
	}
	va_end(ap);
	exit(1);
}

//pour debug permet de check s'il y a des char non printables qui posent pb
//prends une string de bytes et convertie en hex
char	*bin2hex(const unsigned char *input, size_t len)
{
	char	*result;
	char	hexits[] = "0123456789ABCDEF";

	if (input == NULL || len <= 0)
		return (NULL);

	int resultlength = (len * 3) + 1;

	result = (char*)malloc(resultlength);
	bzero(result, resultlength);

	for (int i = 0; i < len; i++) {
		result[i * 3] = hexits[input[i] >> 4];
		result[(i * 3) + 1] = hexits[input[i] & 0x0F];
		result[(i * 3) + 2] = ' ';
	}
	return (result);
}


#endif
