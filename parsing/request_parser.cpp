#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>      // For exit() and EXIT_FAILURE
#include <unistd.h>     // For read
#include <sstream>
#include <stdio.h>
#include <fcntl.h>

typedef struct s_request
{
    std::map<std::string, std::string> headers;
    std::string method;
    std::string path;
    std::string protocol;
    std::vector<std::string> body;
} t_request;

int get_start_line(std::string s, t_request *request)
{
    size_t pos = 0;
    std::string token;
    int j = 0;

    if (s[0] == '/' || s[0] == '#')
        return (1);
    while ((pos = s.find(" ")) != std::string::npos)
    {
        token = s.substr(0, pos);
        if ((token.length()) > 0 && j == 0)
        {
            request->method = token;
            j++;
        }
        else if (token.length() > 0 && j++ == 1)
            request->path = token;
        s.erase(0, pos + 1);
    }
    request->protocol = s;
    return (0);
}

int is_whitespace(std::string line)
{
    int spaces = 0;
    for (std::string::iterator it = line.begin(); it != line.end(); ++it)
    {
        if (isspace(*it))
            spaces++;
        else
            return (EXIT_FAILURE);
    }
    if (spaces == line.length())
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}

int get_headers(std::string line, t_request *request)
{
    size_t pos;

    if (line.empty() || is_whitespace(line) == EXIT_SUCCESS)
        return (EXIT_FAILURE);
    pos = line.find(":");
    if (!(line[0] == '/' || line[0] == '#'))
        request->headers[line.substr(0, pos)] = line.substr(pos + 2, line.length());
    return (EXIT_SUCCESS);
}

int get_body(std::string line, t_request *request)
{
    if (line.empty() || is_whitespace(line) == EXIT_SUCCESS)
        return (EXIT_SUCCESS);
    request->body.push_back(line);
    return (EXIT_SUCCESS);
}

void for_testing_print_request_struct(t_request *request)
{
    std::cout << "\n---START LINE \n";

    std::cout << request->method << " " << request->path << " " << request->protocol << "\n";
    std::map<std::string, std::string>::iterator it;
    std::cout << "\n---HEADERS \n";
    for (it = request->headers.begin(); it != request->headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << '\n';
    }
    std::vector<std::string>::iterator it2;
    std::cout << "\n---BODY \n";
    for (it2 = request->body.begin(); it2 != request->body.end(); ++it2)
    {
        std::cout << *it2 << "\n";
    }
}

int parse_request(t_request *request, int fd)
{
    char buffer[1500];
    std::string line;
    size_t bytesRead = recv(fd, buffer, 1500, 0);
    std::istringstream data(buffer);

    while (data && std::getline(data, line) && get_start_line(line, request))
        ;
    while (data && std::getline(data, line) && get_headers(line, request) == EXIT_SUCCESS)
        ;
    while (data && get_body(line, request) == EXIT_SUCCESS)
        std::getline(data, line);
    for_testing_print_request_struct(request);
    return (1);
}

int main()
{
    // // Create a socket (IPv4, TCP)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        std::cout << "Failed to create socket. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    // Listen to port 9999 on any address
    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(4242); // htons is necessary to convert a number to
                                     // network byte order
    if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
    {
        std::cout << "Failed to bind to port 4242. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // Start listening. Hold at most 10 connections in the queue
    if (listen(sockfd, 10) < 0)
    {
        std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // Grab a connection from the queue
    auto addrlen = sizeof(sockaddr);
    int connection = accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
    if (connection < 0)
    {
        std::cout << "Failed to grab connection. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    t_request request;
    parse_request(&request, connection);
    // char *test = "HTTP/1.1 200 OK\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n    <head>\r\n    <title>Webserv</title>\r\n    </head>\r\n    <body style=\"background-color: black; text-align: center;\">\r\n        <p style=\"padding: 10%; color:aliceblue; font-size: 5rem; \">Hello Webserv</p>\r\n    </body>\r\n</html>";
    char *test = "HTTP/1.1 200 OK\r\nConnection:	close\r\ncontent-type:	text/html\r\nlast-modified:	Mon, 21 Feb 2022 12:48:28 GMT\r\netag:	\"1a78-62138a1c-215ebfbeb5868481;;;\"\r\naccept-ranges:	bytes\r\ncontent-length:	274\r\ndate:	Wed, 02 Mar 2022 18:14:36 GMT\r\nserver:	LiteSpeed\r\ncontent-security-policy:	upgrade-insecure-requests\r\nalt-svc:	h3=\":443\"; ma=2592000, h3-29=\":443\"; ma=2592000, h3-Q050=\":443\"; ma=2592000, h3-Q046=\":443\"; ma=2592000, h3-Q043=\":443\"; ma=2592000, quic=\":443\"; ma=2592000; v=\"43,46\"\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n    <head>\r\n    <title>Webserv</title>\r\n    </head>\r\n    <body style=\"background-color: black; text-align: center;\">\r\n        <p style=\"padding: 10%; color:aliceblue; font-size: 5rem; \">Hello Webserv</p>\r\n    </body>\r\n</html>";
    send(connection, (const void *)test, strlen(test), 0);
    while(1);
    close(connection);
    close(sockfd);
}
