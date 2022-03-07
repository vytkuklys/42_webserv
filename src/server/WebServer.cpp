#include "WebServer.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>
	SERVER::WebServer::WebServer(int domain, int type, int protocol, int port, u_long interface, int backlog): SimpleServer(domain, type, protocol, port, interface, backlog)
	{
		launch();
	}
	
	SERVER::WebServer::~WebServer()
	{

	}

	void SERVER::WebServer::launch()
	{
		while (42)
		{
			std::cout << "Wait for request" << std::endl;
			accepter();
			handler();
			responder();
			std::cout << "finisched" << std::endl;
		}
		
	}

	void SERVER::WebServer::accepter()
	{
		sockaddr_in address = get_socket()->get_addr();
		u_long addrlen = sizeof(address);
		new_socket = accept(get_socket()->get_socket_fd(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
	}

	void SERVER::WebServer::handler()
	{
		request = new Parsing(new_socket, buffer);
		std::cout << buffer << std::endl;
	}

	void SERVER::WebServer::responder()
	{
		std::string test("HTTP/1.1 200 OK\r\nConnection:	close\r\ncontent-type:	text/html\r\nlast-modified:	Mon, 21 Feb 2022 12:48:28 GMT\r\netag:	\"1a78-62138a1c-215ebfbeb5868481;;;\"\r\naccept-ranges:	bytes\r\ncontent-length:	274\r\ndate:	Wed, 02 Mar 2022 18:14:36 GMT\r\nserver:	LiteSpeed\r\ncontent-security-policy:	upgrade-insecure-requests\r\nalt-svc:	h3=\":443\"; ma=2592000, h3-29=\":443\"; ma=2592000, h3-Q050=\":443\"; ma=2592000, h3-Q046=\":443\"; ma=2592000, h3-Q043=\":443\"; ma=2592000, quic=\":443\"; ma=2592000; v=\"43,46\"\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n    <head>\r\n    <title>Webserv</title>\r\n    </head>\r\n    <body style=\"background-color: black; text-align: center;\">\r\n        <p style=\"padding: 10%; color:aliceblue; font-size: 5rem; \">Hello Webserv</p>\r\n    </body>\r\n</html>");
    	send(new_socket, (const void *)test.c_str(), test.length(), 0);
		close(new_socket);
		delete request;
	}

	