#!/usr/bin/python

import os

print("<!DOCTYPE html>")
print("<html lang=\"en\">")
print("<head>")
print("    <meta charset=\"UTF-8\">")
print("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
print("    <meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">")
print("    <title>CGI Script</title>")
print("    <link rel=\"stylesheet\" href=\"/index/styles.css\">")
print("</head>")
print("<body>")
print("<h1>Environment Variables</h1>")
print("<ul>")
for key, value in os.environ.items():
    print("<li><b>%s:</b> %s</li>" % (key, value))
print("</ul>")
print("</body></html>")