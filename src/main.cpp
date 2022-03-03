#include "../inc/Config.hpp"
#include "../inc/Parsing.hpp"

int main (int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Invalid input." << std::endl;
		return (-1);
	}
	Config config(argv[1]);

	config.getServerName();
	config.getErrorPage();
	config.getPort();
	config.getBodySize();



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


	return (0);
}
