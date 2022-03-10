#include "WebServerMac.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>


SERVER::WebServer::WebServer(int domain, int type, int protocol, int port, u_long interface, int backlog) : SimpleServer(domain, type, protocol, port, interface, backlog)
{
	launch();
}

SERVER::WebServer::~WebServer()
{
}

void SERVER::WebServer::launch()
{
	fd_set tmp_read_sockets;
	fd_set tmp_write_sockets;
	int listener = get_socket()->get_socket_fd();
	std::cout << "Listening on port 8080" << std::endl;
	FD_ZERO(&current_sockets);		// init fd set
	FD_SET(listener, &current_sockets); // add listener to the fd set

	while (42)
	{
		tmp_read_sockets = current_sockets;
		tmp_write_sockets = current_sockets; // tmp copy of fd set, because select function destroys second argument
		if (select(FD_SETSIZE + 1, &tmp_read_sockets, &tmp_write_sockets, NULL, NULL) < 0) // should a timer be set at 5th argument?
		{
			perror("Error");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &tmp_read_sockets)) // fd is ready to be read if true
			{
				tmp_socket_fd = i;
				handler();
				std::cout << "read" << i << std::endl;
			}
			else if(FD_ISSET(i, &tmp_write_sockets)) // fd is ready to be written if true
			{			
				tmp_socket_fd = i;
				std::cout << "write to " << i << std::endl;
				responder();
			}
		}
		std::cout << "finished" << std::endl;
	}
}

void SERVER::WebServer::handler()
{
	if (tmp_socket_fd == get_socket()->get_socket_fd())
		handle_new_client();
	else // data from an existing connection, receive it
		handle_known_client();
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
	memset(buffer, 0, 1500);
	numbytes = recv(tmp_socket_fd, buffer, 1500, 0);
	if (numbytes == -1)
		std::cout << "recv error\n";
	else if (numbytes == 0) // connection closed by client
	{
		std::cout << "Socket closed by client " << tmp_socket_fd << "\n";
		FD_CLR(tmp_socket_fd, &current_sockets);
		close(tmp_socket_fd);
		return;
	}
	Parsing info(buffer);
	std::cout << "insert:" << tmp_socket_fd << std::endl;
	data.insert(std::pair<int, Parsing>(tmp_socket_fd, info));
}

void SERVER::WebServer::responder()
{
	std::stringstream client;
	std::map<int, Parsing>::iterator itr = data.find(tmp_socket_fd);
	if (itr != data.end())
	{
		Parsing &info = itr->second; // all header information from client;
		respond_header(client, info);
		if (send(tmp_socket_fd, (const void *)client.str().c_str(), client.str().length(), 0) == -1)
			std::cout << "responder\n";
		client.str(std::string());
		FD_CLR(tmp_socket_fd, &current_sockets); 	 // removes fd from fd set
		data.erase(itr);
		close(tmp_socket_fd);
	}
}


void SERVER::WebServer::respond_header(std::stringstream& client, Parsing& info)
{
	time_t rawtime = time(NULL);
	struct tm* ptm = gmtime(&rawtime);
	client << info.get("protocol") << " 200 OK\r\n"
			<< "Connection:	close\r\n"
			<< "content-type:	text/html\r\n"
			<< "last-modified:	" << http_time(ptm)
			<< "etag:	\"1a78-62138a1c-215ebfbeb5868481;;;\"\r\n"
			<< "accept-ranges:	bytes\r\n"
			<< "content-length:	274\r\n"
			<< "date:	Wed, 02 Mar 2022 18:14:36 GMT\r\n"
			<< "server:	LiteSpeed\r\n"
			<< "content-security-policy:	upgrade-insecure-requests\r\n"
			<< "alt-svc:	h3=\":443\"; ma=2592000, h3-29=\":443\"; ma=2592000, h3-Q050=\":443\"; ma=2592000, h3-Q046=\":443\"; ma=2592000, h3-Q043=\":443\"; ma=2592000, quic=\":443\"; ma=2592000; v=\"43,46\"\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n    <head>\r\n    <title>Webserv</title>\r\n    </head>\r\n    <body style=\"background-color: black; text-align: center;\">\r\n        <p style=\"padding: 10%; color:aliceblue; font-size: 5rem; \">Hello Webserv</p>\r\n    </body>\r\n</html>";
	
}

std::string SERVER::WebServer::http_time(const struct tm *timeptr)
{
	static const std::string wday_name[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const std::string mon_name[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun","Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	std::stringstream date;
	date 	<< wday_name[timeptr->tm_wday]
			<< ", " << timeptr->tm_mday << " " 
			<< mon_name[timeptr->tm_mon] << " "
			<< 2000 + timeptr->tm_year << " "
			<< timeptr->tm_hour << ":"
			<< timeptr->tm_min << ":"
			<< timeptr->tm_sec << " GMT\r\n";
	return(date.str());	
}