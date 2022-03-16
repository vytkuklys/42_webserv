#include "WebServerMac.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>

SERVER::WebServer::WebServer(int domain, int type, int protocol, std::vector<int> &ports, u_long interface, int backlog) :
SimpleServer(domain, type, protocol, ports, interface, backlog)
{

	FD_ZERO(&current_sockets); // init fd set
	for (std::vector<SOCKET::ListenSocket *>::iterator socket = get_sockets().begin(); socket != get_sockets().end(); ++socket)
	{
		listeners.push_back((*socket)->get_socket_fd());
		FD_SET(listeners.back(), &current_sockets); // add listener to the fd set
	}
	launch(ports);
}

SERVER::WebServer::~WebServer()
{
}

void SERVER::WebServer::launch(std::vector <int> &ports)
{
	fd_set tmp_read_sockets;
	fd_set tmp_write_sockets;

	std::vector<int>::iterator it = ports.begin();
	std::vector<int>::iterator ite = ports.end();

	std::cout << "Listening on port(s):";

	for(; it != ite; ++it)
		std::cout << " " << *it;

	std::cout << std::endl;

	while (42)
	{
		tmp_read_sockets = current_sockets;
		tmp_write_sockets = current_sockets;										   // tmp copy of fd set, because select function destroys second argument
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
				// std::cout << "read" << i << std::endl;
				handler();
			}
			else if (FD_ISSET(i, &tmp_write_sockets)) // fd is ready to be written if true
			{
				tmp_socket_fd = i;
				responder();
			}
		}
	}
}

void SERVER::WebServer::handler()
{
	if (std::find(listeners.begin(), listeners.end(), tmp_socket_fd) != listeners.end()) //check if it is in linsteners vector array
		handle_new_client();
	else // data from an existing connection, receive it
		handle_known_client();
}

void SERVER::WebServer::handle_new_client()
{
	std::cout << "handle_new_client" << std::endl;
	accepter();
	FD_SET(tmp_socket_fd, &current_sockets);
	fcntl(tmp_socket_fd, F_SETFL, O_NONBLOCK);
	// std::cout << "handel_new_client\n";
}

void SERVER::WebServer::accepter()
{
	u_long addrlen = sizeof(struct sockaddr_storage);
	tmp_socket_fd = accept(tmp_socket_fd, (struct sockaddr *)&tmp_client_saddr, (socklen_t *)&addrlen); // copy the listern file fd the new fd is responsible for data transfer
}

void SERVER::WebServer::handle_known_client()
{
	Parsing info(tmp_socket_fd);
	//std::cout << "handel_known_client " << tmp_socket_fd << std::endl;
	// std::cout << "insert:" << tmp_socket_fd << std::endl;
	data.insert(std::pair<int, Parsing>(tmp_socket_fd, info));
}

void SERVER::WebServer::responder()
{
	std::string http_response;
	std::map<int, Parsing>::iterator itr = data.find(tmp_socket_fd);
	if (itr != data.end())
	{
		std::cout << "respons" << std::endl;
		int total;
		Parsing &info = itr->second; // all header information from client
		Response response(info);
		http_response = response.get_http_response();
		total = http_response.length();
		// std::cout << http_response  << std::endl;
		const char *ptr = static_cast<const char *>(http_response.c_str());
		while (total > 0)
		{ // larger responses are not sent at once
			int bytes = send(tmp_socket_fd, (const void *)ptr, total, 0);
			// std::string path = response.get_path(); //not nesaserry maybe
			if (bytes == -1)
				std::cout << "responder\n";
			ptr += bytes;
			total -= bytes;
		}
		FD_CLR(tmp_socket_fd, &current_sockets); // removes fd from fd set
		data.erase(itr);
		close(tmp_socket_fd);
	}
}