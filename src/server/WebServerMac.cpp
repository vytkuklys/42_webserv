#include "WebServerMac.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>

SERVER::WebServer::WebServer(std::vector<int> &ports, Config data) : SimpleServer(data.getDomain(), data.getType(), data.getProtocol(), ports, data.getInterface(), data.getBacklog()), config(data)
{
	FD_ZERO(&current_sockets); // init fd set
	FD_ZERO(&write_sockets);
 	LocationData * see = test("yes.com");
	std::cout << see->getLocation();
	for (std::vector<SOCKET::ListenSocket *>::iterator socket = get_sockets().begin(); socket != get_sockets().end(); ++socket)
	{
		listeners.push_back((*socket)->get_socket_fd());
		FD_SET(listeners.back(), &current_sockets); // add listener to the fd set
	}
	launch(ports);
}

int SERVER::WebServer::get_truncated_location(std::vector<std::string> locations, std::string paths)
{
	while(!paths.empty() && paths != "/")
	{
		unsigned long pos = paths.find_last_of("/");
		if (pos == std::string::npos)
			return (-1);
		if (static_cast<int>(pos) == 0)
			paths = "/";
		else
			paths = paths.substr(0, pos);
		std::vector<std::string>::iterator it = locations.begin();
		std::vector<std::string>::iterator ite = locations.end();
		int index = 0;
		while (it != ite)
		{
			std::cout  << *it << ", " << paths << "\n";
			if (*it == paths) 
			{
				return (index);
			}
			++it;
			++index;
		}
	}
	return (-1);
}

LocationData * SERVER::WebServer::test(std::string name)
{
	std::string paths("/upload.html");
	std::vector<std::string> locations;
	std::vector<ConfigData *> data = config.getContConfigData();
	std::vector<ConfigData *>::iterator it = data.begin();
	std::vector<ConfigData *>::iterator ite = data.end();

	while (it != ite)
	{
		std::string server_name = (*it)->getServerName();
		std::vector<LocationData *> locationData = (*it)->getContLocationData();
		std::vector<LocationData *>::iterator it2 = locationData.begin();
		std::vector<LocationData *>::iterator ite2 = locationData.end();
		while (server_name == name && it2 != ite2)
		{
			std::string location = (*it2)->getLocation();
			locations.push_back(location);
			if (paths == location)
			{
				std::cout << "FOUND: " << paths << ", LOCATION: " << location << "\n";
				return (*it2);
			}
			++it2;
		}
		if (server_name == name)
			break;
		++it;
	}
	int res = get_truncated_location(locations, paths);
	std::vector<LocationData *> locationData = (*it)->getContLocationData();
	std::vector<LocationData *>::iterator it2 = locationData.begin();
	std::vector<LocationData *>::iterator ite2 = locationData.end();
	while (res-- > 0 && it2 != ite2)
	{
		if (res == 0)
			return (*it2);
		++it2;
	}
	return (*ite2);
}

SERVER::WebServer::~WebServer()
{
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
				if (i == 7)
				{
					std::cout << "W,";
				}
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
	fcntl(tmp_socket_fd, F_SETFL, O_NONBLOCK);
	FD_SET(tmp_socket_fd, &current_sockets);
	std::cout << "\nhandel_new_client - " << tmp_socket_fd << "\n";
}

void SERVER::WebServer::accepter()
{
	u_long addrlen = sizeof(struct sockaddr_storage);
	tmp_socket_fd = accept(tmp_socket_fd, (struct sockaddr *)&tmp_client_saddr, (socklen_t *)&addrlen); // copies the listen fd, the new fd is responsible for data transfer
}

void SERVER::WebServer::handle_known_client()
{
	if (tmp_socket_fd == 7)
	{
		std::cout << "testing window\n";
	}
	std::map<int, Parsing>::iterator itr = data.find(tmp_socket_fd);
	if (itr == data.end())
	{
		Parsing request(tmp_socket_fd);
		data.insert(std::pair<int, Parsing>(tmp_socket_fd, request));
		FD_SET(tmp_socket_fd, &write_sockets);
	}
	else if (itr != data.end())
	{
		Parsing &res = itr->second;
		if (res.is_chunked())
		{
			res.set_chunked_body(tmp_socket_fd);
		}
		else
		{
			res.set_regular_body(tmp_socket_fd);
		}
	}
}

void SERVER::WebServer::responder()
{
	std::string http_response;
	std::map<int, Parsing>::iterator itr = data.find(tmp_socket_fd);
	if (itr != data.end())
	{
		std::cout << "Response - " << tmp_socket_fd << "\n";
		int total;
		Parsing &info = itr->second; // all header information from client
		{
			Config test(config);
		}
		Response response(info);
		http_response = response.get_http_response();
		total = http_response.length();
		const char *ptr = static_cast<const char *>(http_response.c_str());
		while (total > 0)
		{ // larger responses are not sent at once
			int bytes = send(tmp_socket_fd, (const void *)ptr, total, 0);
			if (bytes == -1)
				std::cout << "responder\n";
			ptr += bytes;
			total -= bytes;
		}
		FD_CLR(tmp_socket_fd, &current_sockets); // removes fd from fd set
		FD_CLR(tmp_socket_fd, &write_sockets);	 // removes fd from fd set
		data.erase(itr);
		close(tmp_socket_fd);
	}
}