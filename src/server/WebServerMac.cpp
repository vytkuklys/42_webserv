#include "WebServerMac.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>

SERVER::WebServer::WebServer(std::vector<int> &ports, Config data) : SimpleServer(data.getDomain(), data.getType(), data.getProtocol(), ports, data.getInterface(), data.getBacklog()), config(data)
{
	FD_ZERO(&current_sockets); // init fd set
	FD_ZERO(&write_sockets);
	for (std::vector<SOCKET::ListenSocket *>::iterator socket = get_sockets().begin(); socket != get_sockets().end(); ++socket)
	{
		listeners.push_back((*socket)->get_socket_fd());
		FD_SET(listeners.back(), &current_sockets); // add listener to the fd set
	}
	launch(ports);
}

SERVER::WebServer::~WebServer()
{
	return;
}

void SERVER::WebServer::launch(std::vector<int> &ports)
{
	fd_set tmp_read_sockets;
	fd_set tmp_write_sockets;

	std::vector<int>::iterator it = ports.begin();
	std::vector<int>::iterator ite = ports.end();

	std::cout << "Listening on port(s):";

	for (; it != ite; ++it)
		std::cout << " " << *it;

	std::cout << std::endl;

	while (42)
	{
		struct timeval timeout;

		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		tmp_read_sockets = current_sockets;
		tmp_write_sockets = write_sockets;
		int len;																				   // tmp copy of fd set, because select function destroys second argument
		if ((len = select(FD_SETSIZE, &tmp_read_sockets, &tmp_write_sockets, NULL, &timeout)) < 0) // should a timer be set at 5th argument?
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
	if (std::find(listeners.begin(), listeners.end(), tmp_socket_fd) != listeners.end()) // check if it is in linsteners vector array
	{
		handle_new_client();
	}
	else // data from an existing connection, receive it
		handle_known_client();
}

void SERVER::WebServer::handle_new_client()
{
	accepter();
	if (tmp_socket_fd != -1)
	{
		fcntl(tmp_socket_fd, F_SETFL, O_NONBLOCK);
		FD_SET(tmp_socket_fd, &current_sockets);
	}
}

void SERVER::WebServer::accepter()
{
	u_long addrlen = sizeof(struct sockaddr_storage);
	tmp_socket_fd = accept(tmp_socket_fd, (struct sockaddr *)&tmp_client_saddr, (socklen_t *)&addrlen); // copies the listen fd, the new fd is responsible for data transfer
}

void SERVER::WebServer::handle_known_client()
{
	std::map<int, Request>::iterator itr = data.find(tmp_socket_fd);
	if (itr == data.end())
	{
		Request req(tmp_socket_fd);
		//logic: if an error occurs fd is removed from the read list and added to the write list. Then response is send with an appropriate status code and html file
		//possible limitation: not emptying the file descriptor
		if (req.get_error_status())
			FD_CLR(tmp_socket_fd, &current_sockets);
		data.insert(std::pair<int, Request>(tmp_socket_fd, req));
		// if (req.get_parsing_position() == first_body && !req.is_chunked() )
		// 	FD_SET(tmp_socket_fd, &write_sockets);
		if (req.get_method() != "POST" && req.get_method() != "PUT")
			FD_SET(tmp_socket_fd, &write_sockets);
		if(req.get_parsing_position() == done)
			FD_SET(tmp_socket_fd, &write_sockets);
	}
	else if (itr->second.get_parsing_position() < first_body)
	{
		itr->second.fill_header(tmp_socket_fd);
		if (itr->second.get_parsing_position() == first_body)
			FD_SET(tmp_socket_fd, &write_sockets);
		// if(itr->second.get_parsing_position() == done)
		// 	FD_SET(tmp_socket_fd, &write_sockets);
	}
	else if ((itr->second.get_method() == "POST" || itr->second.get_method() == "PUT") && itr != data.end())
	{
		Request &req = itr->second;
		if (req.is_chunked())
		{
			std::cout << "known chuked" << std::endl;
			req.set_chunked_body(tmp_socket_fd);
			if(req.get_parsing_position() == done)
				FD_SET(tmp_socket_fd, &write_sockets);
		}
		else
		{
			req.set_regular_body(tmp_socket_fd);
			if (req.get_error_status())
				FD_CLR(tmp_socket_fd, &current_sockets);
		}
	}
	else
	{
		char tmp[1000];
		read(tmp_socket_fd, tmp, 1000);
		// lseek( tmp_socket_fd, 0, SEEK_END);
	}
}

void SERVER::WebServer::responder()
{
	std::string http_response;
	std::map<int, Request>::iterator itr = data.find(tmp_socket_fd);
	if (itr != data.end())
	{
		int total;
		Request &info = itr->second;
		Response response(info, config);
		http_response = response.get_http_response();
		total = http_response.length();
		const char *ptr = static_cast<const char *>(http_response.c_str());
		while (total > 0)
		{
			int bytes = send(tmp_socket_fd, (const void *)ptr, total, 0);
			if (bytes == -1 || bytes == 0)
				break ; //HTTP server closes the socket if an error occurs during the sending of a file
			ptr += bytes;
			total -= bytes;
		}
		FD_CLR(tmp_socket_fd, &current_sockets);
		FD_CLR(tmp_socket_fd, &write_sockets);
		data.erase(itr);
		close(tmp_socket_fd);
	}
}