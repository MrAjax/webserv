#!/usr/bin/env python3

import os
import cgi
import csv
import html
from urllib.parse import parse_qs
from datetime import datetime

def print_html_page(username):
    file_path = 'website_exmpl/Zzewebsite/admin/welcome/created.html'
    try:
        with open(file_path, 'r') as file:
            for line in file:
                print(line.replace('USER', username))
    except FileNotFoundError:
        print("X")

def create_user(username, password):
    data_file = 'website_exmpl/Zzewebsite/user/welcome/welcome_data.csv'
    creation_date = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    try:
        with open(data_file, mode='a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([username, password, creation_date])
    except FileNotFoundError:
        print("X")

query_string = os.environ.get("QUERY_STRING", "")
params = parse_qs(query_string)
username = params.get("username", [None])[0]
password = params.get("password", [None])[0]

if username and password:
    create_user(username, password)
    print_html_page(username)
else:
    print("X")