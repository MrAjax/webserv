#!/usr/bin/env python3

import os
import cgi
import csv
import html
from urllib.parse import parse_qs
from datetime import datetime
import email.utils

def print_http_response(body):
    if body[0] !=  'X':
        http_date = email.utils.formatdate(usegmt=True)
        server_name = "Webserv"
        content_type = "text/html"
        content_length = len(body.encode('utf-8'))
        
        headers = [
        f"HTTP/1.1 200 OK",
        f"Date: {http_date}",
        f"Server: {server_name}",
        f"Content-Type: {content_type}",
        f"Content-Length: {content_length}",
        ]
        
        http_response = "\r\n".join(headers) + "\r\n\r\n" + body
        print(http_response)
    else:
        print("X")

def print_html_page(username, body):
    file_path = 'website_exmpl/Zzewebsite/admin/welcome/created.html'
    try:
        with open(file_path, 'r') as file:
            for line in file:
                body += (line.replace('USER', username))
    except FileNotFoundError:
        body = "X"
    return body

def create_user(username, password, body):
    data_file = 'website_exmpl/Zzewebsite/user/welcome/welcome_data.csv'
    creation_date = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    try:
        with open(data_file, mode='a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([username, password, creation_date])
    except FileNotFoundError:
        body = "X"
    return body

query_string = os.environ.get("QUERY_STRING", "")
params = parse_qs(query_string)
username = params.get("username", [None])[0]
password = params.get("password", [None])[0]
body = ''

if username and password:
    body = create_user(username, password, body)
    body = print_html_page(username, body)
else:
    body = "X"
print_http_response(body)