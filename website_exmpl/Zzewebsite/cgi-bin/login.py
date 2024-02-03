#!/usr/bin/env python3

import os
import cgi
import csv
import html
from urllib.parse import parse_qs

def check_credentials(username, password, filename="website_exmpl/Zzewebsite/user/welcome/welcome_data.csv"):
    with open(filename, newline='') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            if row and row[0]:
                if row[0] == username and row[1] == password:
                    return True
    return False

def print_html_page(username):
    file_path = 'website_exmpl/Zzewebsite/user/welcome/welcome.html'
    try:
        with open(file_path, 'r') as file:
            for line in file:
                print(line.replace('USER', username))
    except FileNotFoundError:
        print("X")

query_string = os.environ.get("QUERY_STRING", "")
params = parse_qs(query_string)
username = params.get("username", [None])[0]
password = params.get("password", [None])[0]

if username and password:
    if check_credentials(username, password):
        print_html_page(username)
    else:
        print("Invalid credentials")
else:
    print("X")