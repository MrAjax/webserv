#!/usr/bin/env python3

import os
import cgi
import csv
import html
from urllib.parse import parse_qs
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

def generate_html_rows():
    filename = 'website_exmpl/Zzewebsite/data/welcome_data.csv'
    rows = ""
    try:
        with open(filename, newline='') as csvfile:
            reader = csv.reader(csvfile)
            for i, row in enumerate(reader, start=1):
                rows += f'<tr><td>{i}</td><td>{row[0]}</td><td>{row[2]}</td></tr>\n'
    except FileNotFoundError:
        rows = ""
    return(rows)

def print_html_page():
    file_path = 'website_exmpl/Zzewebsite/admin/welcome/user_management.html'
    try:
        with open(file_path, 'r') as file:
            file_content = file.read()
        data_rows = generate_html_rows()
        if data_rows == "X":
            output = "X"
        else:
            output = file_content.replace('{rows}', data_rows)
    except FileNotFoundError:
        output = "X"
    return output

body = print_html_page()
print_http_response(body)
