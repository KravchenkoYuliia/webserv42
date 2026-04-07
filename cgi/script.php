#!/usr/bin/php-cgi

<?php
// Sleep for 5 seconds
sleep(5);

// ---- Headers (MANDATORY FORMAT) ----
print "Content-Type: text/html\r\n";
print "\r\n";

// ---- Body ----
print "<html>";
print "<head><title>CGI Test</title></head>";
print "<body>";

print "<h1>CGI Script</h1>";
print "<h2>Finished sleeping for 5 seconds</h2>";

print "<h3>Environment Variables:</h3>";
print "<ul>";

# List of variables set in CgiHandler::execCGI through CgiHandler::buildEnvironmentVariables
$keys = [
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
];

foreach ($keys as $key) {
    $value = getenv($key);
    if ($value === false) {
        $value = "(not set)";
    }
    print "<li><b>$key</b>: $value</li>";
}

print "</ul>";

print "</body>";
print "</html>";

// Flush output
flush();
?>
