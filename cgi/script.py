#!/usr/bin/python3
import sys
import time
import os

time.sleep(5)

# ---- Headers (MANDATORY FORMAT) ----
print("Content-Type: text/html\r\n")
print("\r\n")

# ---- Body ----
print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")

print("<h1>CGI Script</h1>")
print("<h2>Finished sleeping for 5 seconds</h2>")

print("<h3>Environment Variables:</h3>")
print("<ul>")

# List of variables set in CgiHandler::execCGI through CgiHandler::buildEnvironmentVariables
keys = [
    "GATEWAY_INTERFACE",
    "SERVER_PROTOCOL",
    "SERVER_SOFTWARE",
    "REQUEST_METHOD",
    "SCRIPT_NAME",
    "PATH_INFO",
    "CONTENT_LENGTH",
    "CONTENT_TYPE",
    "REDIRECT_STATUS",
    "SCRIPT_FILENAME",
]

for key in keys:
    value = os.environ.get(key, "(not set)")
    print(f"<li><b>{key}</b>: {value}</li>")

print("</ul>")

print("</body>")
print("</html>")

sys.stdout.flush()
