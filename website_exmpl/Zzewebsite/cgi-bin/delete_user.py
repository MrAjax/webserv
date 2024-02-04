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
    file_path = 'website_exmpl/Zzewebsite/admin/welcome/delete.html'
    try:
        with open(file_path, 'r') as file:
            for line in file:
                body += line.replace('USER', username)
    except FileNotFoundError:
        body = "X"
    return  body

def print_user_not_found(username, body):
    file_path = 'website_exmpl/Zzewebsite/admin/welcome/delete.html'
    try:
        with open(file_path, 'r') as file:
            for line in file:
                line = line.replace('USER', username)
                line = line.replace('was deleted', 'does not exist')
                line = line.replace('/admin/welcome/delete.mp4', '/admin/welcome/sorry.mp4')
                body += line
    except FileNotFoundError:
        body = "X"
    return  body

def delete_user(username):
    data_file = 'website_exmpl/Zzewebsite/user/welcome/welcome_data.csv'
    temp_file = 'website_exmpl/Zzewebsite/user/welcome/temp_welcome_data.csv'
    found_user = False
    try:
        with open(data_file, mode='r', newline='') as infile, open(temp_file, 'w', newline='') as outfile:
            reader = csv.reader(infile)
            writer = csv.writer(outfile)
            for row in reader:
                if row[0] == username:
                    found_user = True
                else:
                    writer.writerow(row)
        os.replace(temp_file, data_file)
    except FileNotFoundError:
        return "X"
    if found_user == True:
        return ''
    else:
        return 'o'

query_string = os.environ.get("QUERY_STRING", "")
params = parse_qs(query_string)
username = params.get("username", [None])[0]
body = ''

if username:
    body = delete_user(username)
    if (body and body[0] == 'o'):
        body = print_user_not_found(username, '')
    else:
        body += print_html_page(username, body)
else:
    body = print("X")
print_http_response(body)