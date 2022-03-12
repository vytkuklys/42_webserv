#include "WebServerMac.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <poll.h>


SERVER::WebServer::WebServer(int domain, int type, int protocol, int port, u_long interface, int backlog) : SimpleServer(domain, type, protocol, port, interface, backlog)
{
	launch();
}

SERVER::WebServer::~WebServer()
{
}

void SERVER::WebServer::launch()
{
	int j;
	int listener = get_socket()->get_socket_fd();
	struct pollfd tmp = {fd = listener, events = POLLIN, revents = 0};
	std::vector<struct pollfd> pollfds = {tmp};
	numfds = 1;
	std::cout << "Listening on port 8080" << std::endl;

	while (42)
	{
		if (poll(pollfds.begin().base(), numfds, -1) < 0) // should a timer be set at 5th argument?
		{
			perror("Error\n");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < numfds; i++)
		{
			if (pollfds[i].fd <= 0)
			{
				j++;
				if(j > numfds / 2)
				{
					std::vector<struct pollfd> pollfds_tmp(pollfds);
					pollfds.clear();
					for (std::vector<pollfd>::iterator tmp = pollfds_tmp.begin(); tmp < pollfds.end(); tmp++)
					{
						if(tmp.base()->fd != -1)
							pollfds.push_back(*tmp);
					}
					pollfds_tmp.clear();
					j = 0;
				}
				continue;
			}
			if((pollfds[i].revents & POLLIN) == POLLIN)
			{
				tmp_socket = &pollfds[i];
				handler();
			}
		}
		std::cout << "finished" << std::endl;
	}
}

void SERVER::WebServer::handler()
{
	if (tmp_socket->fd == get_socket()->get_socket_fd())
		handle_new_client();
	else // data from an existing connection, receive it
	{
		handle_known_client();
		// close(tmp_socket_fd)
		tmp_socket->fd = -1;
		// removes fd from fd set
	}
}

void SERVER::WebServer::handle_new_client()
{
	accepter();
	FD_SET(tmp_socket_fd, &current_sockets);
	std::cout << "handel_new_client\n";
}

void SERVER::WebServer::accepter()
{
	u_long addrlen = sizeof(struct sockaddr_storage);
	tmp_socket_fd = accept(get_socket()->get_socket_fd(), (struct sockaddr *)&tmp_client_saddr, (socklen_t *)&addrlen);
}

void SERVER::WebServer::handle_known_client()
{
	std::cout << "handel_known_client\n";
	ssize_t numbytes;
	numbytes = recv(tmp_socket_fd, buffer, 1500, 0);
	if (numbytes == -1)
		std::cout << "recv error\n";
	else if (numbytes == 0) // connection closed by client
	{
		std::cout << "Socket closed by client " << tmp_socket_fd << "\n";
		FD_CLR(tmp_socket_fd, &current_sockets);
	}
	Parsing info(buffer);
	responder();
}

void SERVER::WebServer::responder()
{
	std::cout << "responder\n";
	std::string test("HTTP/1.1 200 OK\r\nConnection:	close\r\ncontent-type:	text/html\r\nlast-modified:	Mon, 21 Feb 2022 12:48:28 GMT\r\netag:	\"1a78-62138a1c-215ebfbeb5868481;;;\"\r\naccept-ranges:	bytes\r\ncontent-length:	274\r\ndate:	Wed, 02 Mar 2022 18:14:36 GMT\r\nserver:	LiteSpeed\r\ncontent-security-policy:	upgrade-insecure-requests\r\nalt-svc:	h3=\":443\"; ma=2592000, h3-29=\":443\"; ma=2592000, h3-Q050=\":443\"; ma=2592000, h3-Q046=\":443\"; ma=2592000, h3-Q043=\":443\"; ma=2592000, quic=\":443\"; ma=2592000; v=\"43,46\"\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n    <head>\r\n    <title>Webserv</title>\r\n    </head>\r\n    <body style=\"background-color: black; text-align: center;\">\r\n        <p style=\"padding: 10%; color:aliceblue; font-size: 5rem; \">Hello Webserv</p>\r\n    </body>\r\n</html>");
	if (send(tmp_socket_fd, (const void *)test.c_str(), test.length(), 0) == -1)
		std::cout << "responder\n";
}
