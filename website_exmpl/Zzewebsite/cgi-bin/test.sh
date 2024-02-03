#!/bin/bash

if [ -n "$QUERY_STRING" ]; then
    IFS='=' read -r key value <<< "$QUERY_STRING"

    if [ "$key" = "name" ] && [ -n "$value" ]; then
        echo "<h1>Hello, $value!</h1>"
    else
        echo "<h1>Hello, world!</h1>"
    fi
else
    echo "<h1>Hello, world!</h1>"
fi
