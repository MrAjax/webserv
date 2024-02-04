#!/usr/bin/env python3

import os
import cgi
import csv
import html
from urllib.parse import parse_qs
from datetime import datetime

def print_html_page(username):
    file_path = 'website_exmpl/Zzewebsite/admin/welcome/delete.html'
    try:
        with open(file_path, 'r') as file:
            for line in file:
                print(line.replace('USER', username))
    except FileNotFoundError:
        print("Xx")

def delete_user(username):
    data_file = 'website_exmpl/Zzewebsite/user/welcome/welcome_data.csv'
    temp_file = 'website_exmpl/Zzewebsite/user/welcome/temp_welcome_data.csv'
    try:
        with open(data_file, mode='r', newline='') as infile, open(temp_file, 'w', newline='') as outfile:
            reader = csv.reader(infile)
            writer = csv.writer(outfile)
            for row in reader:
                if row[0] != username:
                    writer.writerow(row)
        os.replace(temp_file, data_file)
    except FileNotFoundError:
        print("Xxx")

query_string = os.environ.get("QUERY_STRING", "")
params = parse_qs(query_string)
username = params.get("username", [None])[0]

if username:
    delete_user(username)
    print_html_page(username)
else:
    print("X")