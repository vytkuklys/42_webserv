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
		int listener = get_socket()->get_socket_fd();
		if ((efd = epoll_create(42)) == -1) //creates a new epoll instance
		{
			std::cout << "epoll_create" << std::endl;
			exit(EXIT_FAILURE);
		}
		ev.events = EPOLLIN;
		ev.data.fd = listener;
		if (epoll_ctl(efd, EPOLL_CTL_ADD, listener, &ev) == -1)
		{
			std::cout << "epoll_ctl error" << std::endl;
			exit(EXIT_FAILURE);
		}

		struct epoll_event ep_event[MAX_EVENTS];
		int nfds = 0;
		while (42)
		{
			std::cout << "Wait for request" << std::endl;
			if ((nfds = epoll_wait (efd, ep_event, MAX_EVENTS, -1)) == -1)
			{
				std::cout << "epoll_wait" << std::endl;
				exit(EXIT_FAILURE);
			}
			for (int i = 0; i < nfds; i++)
			{
				if ((ep_event[i].events & EPOLLIN) == EPOLLIN)
				{
					tmp_socket_fd = ep_event[i].data.fd;
					handler();
				}
			}
			std::cout << "finisched" << std::endl;
		}
		
	}

	void SERVER::WebServer::handler()
	{
		if(tmp_socket_fd == get_socket()->get_socket_fd())
			handle_new_client();
		else // data from an existing connection, receive it
			handle_known_client();
	}

	void SERVER::WebServer::handle_new_client()
	{
		struct epoll_event ev;
	
		accepter();
		ev.events = EPOLLIN;
		ev.data.fd = tmp_socket_fd;
		if (epoll_ctl(efd, EPOLL_CTL_ADD, tmp_socket_fd, &ev) == -1)
		{
			std::cout << "epoll_create" << std::endl;
			exit(EXIT_FAILURE);	
		}
		std::cout << "handel_new_clienrt\n";
	}

	void SERVER::WebServer::accepter()
	{
		u_long addrlen = sizeof(struct sockaddr_storage);
		tmp_socket_fd = accept(get_socket()->get_socket_fd(), (struct sockaddr *)&tmp_client_saddr, (socklen_t *)&addrlen);
	}

	void SERVER::WebServer::handle_known_client()
	{
		ssize_t numbytes;
		numbytes = recv(tmp_socket_fd, buffer, 1500, 0);
		if (numbytes == -1)
			std::cout << "recv error\n";
		else if (numbytes == 0) // connection closed by client
		{
			std::cout << "Socket closed by client " << tmp_socket_fd <<"\n";
			if (epoll_ctl (efd, EPOLL_CTL_DEL, tmp_socket_fd, &ev) == -1)
				std::cout << "epoll_ctl\n";
			if (close (tmp_socket_fd) == -1)
				std::cout << "close\n";
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
		close(tmp_socket_fd);
	}

	