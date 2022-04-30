# 42_webserv

A server that is able to handle multiple requests and return appropriate responses to multiple clients. Optimized for google chrome browser.

## How to run this web server:

- paste the following line to a terminal: ```git clone https://github.com/vytkuklys/42_webserv.git```
- type in terminal: ```cd 42_webserv```
- type in terminal: ```make ```
- type in terminal: ./webserv filepath [e.g. ```./webserv ./config_files/default.conf```]
- type in Google Chrome: ```localhost:8080```

## This Webserv meets the following criteria:
- It takes a configuration file as an argument.
- Server never blocks and the client can be bounced properly if necessary.
- Every read or a write operation is done after going through select().
- Used the following macro and define like FD_SET, FD_CLR, FD_ISSET, FD_ZERO.
- This server is compatible with the Google Chrome.
- HTTP response status codes must are accurate for (400/404/303/200/405).
- Server has a default error pages if none are provided.
- Fork is used only for CGI (PHP and C++).
- Server serves a fully static website.
- Clients are able to upload files.
- Following methods are implemented: GET, POST, DELETE  and PUT.
- Server can handle siege stress test up to 20 users
- Server is able to listen to multiple ports.
