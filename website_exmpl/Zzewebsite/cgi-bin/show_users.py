#!/usr/bin/env python3

import os
import cgi
import csv
import html
from urllib.parse import parse_qs

def generate_html_rows():
    filename = 'website_exmpl/Zzewebsite/user/welcome/welcome_data.csv'
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

print(print_html_page())
