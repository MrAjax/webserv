#!/bin/bash

chmod 700 website_exmpl/Zzewebsite/data/diary_data.csv
echo "HTTP/1.1 303 See Other"
echo "Location: /user/welcome/diary.html"
echo "Content-Length: 2531"
echo ""
echo ""