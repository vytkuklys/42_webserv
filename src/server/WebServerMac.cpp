#include "WebServerMac.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>

SERVER::WebServer::WebServer(std::vector<int> &ports, Config& data) : SimpleServer(data.getDomain(), data.getType(), data.getProtocol(), ports, data.getInterface(), data.getBacklog())
{
	config = &data;
	FD_ZERO(&read_sockets); // init fd set
	FD_ZERO(&write_sockets);
	for (std::vector<SOCKET::ListenSocket *>::iterator socket = get_sockets().begin(); socket != get_sockets().end(); ++socket)
	{
		listeners.push_back((*socket)->get_socket_fd());
		FD_SET(listeners.back(), &read_sockets); // add listener to the fd set
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
		std::cout << "end?" << std::endl;

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		tmp_read_sockets = read_sockets;
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
		FD_SET(tmp_socket_fd, &read_sockets);
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
		std::cout << "constructor" << std::endl;
		Request req(*config);
		data.insert(std::pair<int, Request>(tmp_socket_fd, req));
		itr = data.find(tmp_socket_fd);
	}

	std::cout << "process socket information" << std::endl;
	itr->second.fill_header(tmp_socket_fd);
	if (itr->second.get_error_status())
		FD_CLR(tmp_socket_fd, &read_sockets);
	if (itr->second.get_parsing_position() == send_first)
	{
		std::cout << "set fd to write list" << std::endl;
		FD_SET(tmp_socket_fd, &write_sockets);
	}
	std::cout << "done with know client" << std::endl;
	// std::cout << "tmp fd =" << tmp_socket_fd << "parsing position" << itr->second.get_parsing_position() << std::endl;
}

void SERVER::WebServer::responder()
{
	bool	end_of_chunked = false;

	std::string http_response;
	std::map<int, Request>::iterator itr = data.find(tmp_socket_fd);
	if (itr != data.end())
	{
		int total;
		Request &info = itr->second;
		std::cout << "responder" << std::endl;
		if(info.get_parsing_position() <= send_first)
		{
			std::cout << "send first" << std::endl;
			Response response(info, *config);
			http_response = response.get_http_response();
			info.set_parsing_position(erase_cgi_header);
			FD_CLR(tmp_socket_fd, &read_sockets);
		}
		else
		{
			std::cout << "send body" << std::endl;
			http_response = info.get_cgi_return();
			total = http_response.length();
			std::stringstream stream;
			stream << std::hex << total;
			// if(http_response.find("\r\n") != std::string::npos)
			// 	std::cerr << "why" << std::endl;
			// std::cerr << total << std::endl;
			// std::cerr << "." << http_response << ".";
			// std::cerr << "." << stream.str() << "." << std::endl;
			// std::cerr << "," << http_response << "," << std::endl;
			http_response.insert(0, (stream.str() + "\r\n"));
			// if(http_response.find("\r\n", (stream.str() + "\r\n").length()) != std::string::npos)
			// 	std::cerr << "why" << http_response.find("\r\n", (stream.str() + "\r\n").length()) << std::endl;
			std::cerr << http_response;
			http_response.append("\r\n");
			if(total == 0)
			{
				end_of_chunked = true;
			}
			stream.clear();
		}
		total = http_response.length();
		// const char *ptr = static_cast<const char *>(http_response.c_str());
		while (total > 0)
		{
			int bytes = send(tmp_socket_fd, static_cast<const void *>(http_response.c_str()), total, 0);
			// if (bytes != total)
			// 	std::cerr << "error send " << bytes << "should" << total<< "measage" << http_response << std::endl;
			if (bytes == -1 || bytes == 0)
				break ; //HTTP server closes the socket if an error occurs during the sending of a file
			// ptr += bytes;
			total -= bytes;
		}
		if(!info.is_chunked())
		{
			std::cout << "close socket" << std::endl;
			FD_CLR(tmp_socket_fd, &write_sockets);
			data.erase(itr);
			close(tmp_socket_fd);
		}
		if (end_of_chunked)
		{
			std::cout << "close socket" << std::endl;
			FD_CLR(tmp_socket_fd, &write_sockets);
			data.erase(itr);
			close(tmp_socket_fd);
			return;
		}
	}
}