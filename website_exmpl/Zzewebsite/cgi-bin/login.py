#!/usr/bin/env python3

import os
import cgi
import csv
import html
from urllib.parse import parse_qs
from datetime import datetime, timedelta
import email.utils
import uuid

def print_http_response(body, cookie_id):
    if body and body[0] !=  'X':
        http_date = email.utils.formatdate(usegmt=True)
        server_name = "Webserv"
        content_type = "text/html"
        content_length = len(body.encode('utf-8'))
        
        headers = [
        f"HTTP/1.1 200 OK",
        f"Date: {http_date}",
        f"Server: {server_name}",
        f"Set-Cookie: login-cookie={cookie_id}; HttpOnly; Path=/",
        f"Content-Type: {content_type}",
        f"Content-Length: {content_length}",
        ]
        
        http_response = "\r\n".join(headers) + "\r\n\r\n" + body
        print(http_response)
    else:
        print(body)
        print("Xx-xx")


def check_credentials(username, password, filename="website_exmpl/Zzewebsite/user/welcome/welcome_data.csv"):
    with open(filename, newline='') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            if row and row[0]:
                if row[0] == username and row[1] == password:
                    return True
    return False

def set_cookie(username):
    expiration = datetime.now() + timedelta(minutes=1)
    expiration_date = expiration.strftime('%Y-%m-%d %H:%M:%S')
    cookie_id = str(uuid.uuid4())
    cookie_file = 'website_exmpl/Zzewebsite/user/welcome/cookie_data.csv'
    try:
         with open(cookie_file, mode='a', newline='') as file:
               writer = csv.writer(file)
               writer.writerow([username, cookie_id, expiration_date])
         return cookie_id
    except FileNotFoundError:
    	return 'Xxxx'

def print_html_page(username, body):
    file_path = 'website_exmpl/Zzewebsite/user/welcome/welcome.html'
    try:
        with open(file_path, 'r') as file:
            for line in file:
                body += line.replace('USER', username)
    except FileNotFoundError:
        body = "Xxx"
    return body


def print_login_page(body):
    file_path = 'website_exmpl/Zzewebsite/user/login/login.html'
    try:
        with open(file_path, 'r') as file:
            for line in file:
                line = line.replace('<!-- <div id="error-message" style="color: red;">Invalid credentials</div> -->', '<div id="error-message" style="color: red;">Invalid credentials</div>')
                line = line.replace('/user/login/login.jpg', '/user/login/error.jpg')
                body += line
    except FileNotFoundError:
        body = 'Xx'
    return body

query_string = os.environ.get("QUERY_STRING", "")
params = parse_qs(query_string)
username = params.get("username", [None])[0]
password = params.get("password", [None])[0]
body = ''
cookie = ''

if username and password:
    if check_credentials(username, password):
        body = print_html_page(username, body)
        cookie = set_cookie(username)
    else:
        body = print_login_page(body)
else:
    body = "X"
print_http_response(body, cookie)