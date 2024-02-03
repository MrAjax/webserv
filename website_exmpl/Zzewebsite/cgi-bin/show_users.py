#!/usr/bin/env python3

import os
import cgi
import csv
import html
from urllib.parse import parse_qs

def print_html_page(username):
    file_path = 'website_exmpl/Zzewebsite/admin/welcome/user_management.html'
    try:
        with open(file_path, 'r') as file:
            for line in file:
                print(line)
    except FileNotFoundError:
        print("X")

query_string = os.environ.get("QUERY_STRING", "")
params = parse_qs(query_string)
username = params.get("username", [None])[0]
password = params.get("password", [None])[0]

print_html_page(username)
