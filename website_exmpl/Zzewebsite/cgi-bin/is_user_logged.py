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

def check_cookie(cookie_id, current_time):
    filename="website_exmpl/Zzewebsite/user/welcome/cookie_data.csv"
    #tmp_file="website_exmpl/Zzewebsite/user/welcome/cookie_data_tmp.csv"
    try:
         with open(filename, newline='') as csvfile:
               reader = csv.reader(csvfile)
               for row in reader:
                    if row and row[0]:
                           cookie_time = datetime.strptime(row[2], '%Y-%m-%d %H:%M:%S')
                           if row[1] == cookie_id and cookie_time <= current_time:
                                 return row[0]
         return ''
    except FileNotFoundError:
    	return ''

def print_login_page(body):
    file_path = 'website_exmpl/Zzewebsite/user/login/login.html'
    try:
        with open(file_path, 'r') as file:
            for line in file:
                body += (line)
    except FileNotFoundError:
        body = ("X")
    return body

def print_html_page(username, body):
    file_path = 'website_exmpl/Zzewebsite/user/welcome/welcome.html'
    try:
        with open(file_path, 'r') as file:
            for line in file:
                body += (line.replace('USER', username))
    except FileNotFoundError:
        boddy = ("X")
    return body

query_string = os.environ.get("QUERY_STRING", "")
params = parse_qs(query_string)
cookie_id = params.get("id", [None])[0]
current_time = datetime.now()
username = check_cookie(cookie_id, current_time)
body = ''

if cookie_id and username:
    body += print_html_page(username)  
else:
    body += print_login_page(body)
print_http_response(body)