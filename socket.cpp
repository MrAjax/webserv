#include <sys/socket.h>
#include <iostream>

int	main() 
{
	int	socket_fd;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);  //SOCK_STREAM = TCP
	if (socket_fd < 0)
	{
		std::cerr << "Socket error" << std::endl;
		return 0;
	}
	std::cout << "Socket_fd =" << socket_fd << std::endl;
	return 0;

	
}
