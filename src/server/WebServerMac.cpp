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
		if (select(FD_SETSIZE, &tmp_read_sockets, &tmp_write_sockets, NULL, NULL) < 0) // should a timer be set at 5th argument?
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
				// std::cout << "read" << i << std::endl;
			}
			else if(FD_ISSET(i, &tmp_write_sockets)) // fd is ready to be written if true
			{			
				tmp_socket_fd = i;
				// std::cout << "write to " << i << std::endl;
				responder();
			}
		}
		// std::cout << "finished" << std::endl;
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
	// std::cout << "handel_new_client\n"; 
}

void SERVER::WebServer::accepter()
{
	u_long addrlen = sizeof(struct sockaddr_storage);
	tmp_socket_fd = accept(get_socket()->get_socket_fd(), (struct sockaddr *)&tmp_client_saddr, (socklen_t *)&addrlen);
}

void SERVER::WebServer::handle_known_client()
{
	// std::cout << "handel_known_client\n";
	Parsing info(tmp_socket_fd);
	// std::cout << "insert:" << tmp_socket_fd << std::endl;
	data.insert(std::pair<int, Parsing>(tmp_socket_fd, info));
}

void SERVER::WebServer::responder()
{
	std::string http_response;
	std::map<int, Parsing>::iterator itr = data.find(tmp_socket_fd);
	if (itr != data.end())
	{
		Parsing &info = itr->second; // all header information from client
		Response response(info);
		http_response = response.get_http_response();
		if (send(tmp_socket_fd, (const void *)http_response.c_str(), http_response.length(), 0) == -1)
			std::cout << "responder\n";
		FD_CLR(tmp_socket_fd, &current_sockets); 	 // removes fd from fd set
		data.erase(itr);
		close(tmp_socket_fd);
	}
}
