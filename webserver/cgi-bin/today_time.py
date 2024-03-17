#!/usr/bin/python3

import datetime
import cgi

now = datetime.datetime.now()
formatted_time = now.strftime("%H:%M:%S")
print("HTTP/1.1 200 OK")
print("Content-type: text/html\r\n\r\n<html>\n<head>")
print(f"<h1>{formatted_time}</h1>")