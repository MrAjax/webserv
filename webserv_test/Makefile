
all:
	c++ web_client.cpp -o client
	c++ web_server.cpp -o server

client:
	c++ web_client.cpp -o client

server:
	c++ web_server.cpp HttpRequest.cpp -o server

clean:
	rm -f client
	rm -f server

re: clean all

.PHONY: all clean re server client