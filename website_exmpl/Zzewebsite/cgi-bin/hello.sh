#!/bin/bash

content="<h1>Hello World</h1>"
content_length=${#content}

echo "HTTP/1.1 200 OK"
echo "Server: Webserv"
echo "Content-Type: text/html"
echo "Content-Length: ${content_length}"
echo -e "Connection: keep-alive\r\n\r\n"
echo -e "${content}"
