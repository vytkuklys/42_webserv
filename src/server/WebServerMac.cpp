#include "WebServerMac.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <signal.h>

bool SERVER::WebServer::is_running = true;

SERVER::WebServer::WebServer(Config& data) : SimpleServer(data.getDomain(), data.getType(), data.getProtocol(), data.getPorts(), data.getInterface(), data.getBacklog())
{
	summe = 0;
	config = &data;
	FD_ZERO(&read_sockets); // init fd set
	FD_ZERO(&write_sockets);
	for (std::vector<SOCKET::ListenSocket *>::iterator socket = get_sockets().begin(); socket != get_sockets().end(); ++socket)
	{
		listeners.push_back((*socket)->get_socket_fd());
		FD_SET(listeners.back(), &read_sockets); // add listener to the fd set
	}
	status_line.insert(std::pair<int, std::string>(100, "Continue"));
	status_line.insert(std::pair<int, std::string>(101, "Switching Protocols"));
	status_line.insert(std::pair<int, std::string>(102, "Processing"));
	status_line.insert(std::pair<int, std::string>(103, "Early Hints"));

	status_line.insert(std::pair<int, std::string>(200, "OK"));
	status_line.insert(std::pair<int, std::string>(201, "Created"));
	status_line.insert(std::pair<int, std::string>(202, "Accepted"));
	status_line.insert(std::pair<int, std::string>(203, "Non-Authoritative Information"));
	status_line.insert(std::pair<int, std::string>(204, "No Content"));
	status_line.insert(std::pair<int, std::string>(205, "Reset Content"));
	status_line.insert(std::pair<int, std::string>(206, "Partial Content"));
	status_line.insert(std::pair<int, std::string>(207, "Multi-Status"));
	status_line.insert(std::pair<int, std::string>(208, "Already Reported"));
	status_line.insert(std::pair<int, std::string>(226, "IM Used"));

	status_line.insert(std::pair<int, std::string>(300, "Multiple Choices"));
	status_line.insert(std::pair<int, std::string>(301, "Moved Permanently"));
	status_line.insert(std::pair<int, std::string>(302, "Found (Moved Temporarily)"));
	status_line.insert(std::pair<int, std::string>(303, "See Other"));
	status_line.insert(std::pair<int, std::string>(304, "Not Modified"));
	status_line.insert(std::pair<int, std::string>(305, "Use Proxy"));
	status_line.insert(std::pair<int, std::string>(306, "(reserviert)"));
	status_line.insert(std::pair<int, std::string>(307, "Temporary Redirect"));
	status_line.insert(std::pair<int, std::string>(308, "Permanent Redirect"));

	status_line.insert(std::pair<int, std::string>(400, "Bad Request"));
	status_line.insert(std::pair<int, std::string>(401, "Unauthorized"));
	status_line.insert(std::pair<int, std::string>(402, "Payment Required"));
	status_line.insert(std::pair<int, std::string>(403, "Forbidden"));
	status_line.insert(std::pair<int, std::string>(404, "Not Found"));
	status_line.insert(std::pair<int, std::string>(405, "Method Not Allowed"));
	status_line.insert(std::pair<int, std::string>(406, "Not Acceptable"));
	status_line.insert(std::pair<int, std::string>(407, "Proxy Authentication Required"));
	status_line.insert(std::pair<int, std::string>(408, "Request Timeout"));
	status_line.insert(std::pair<int, std::string>(409, "Conflict"));
	status_line.insert(std::pair<int, std::string>(410, "Gone"));
	status_line.insert(std::pair<int, std::string>(411, "Length Required"));
	status_line.insert(std::pair<int, std::string>(412, "Precondition Failed"));
	status_line.insert(std::pair<int, std::string>(413, "Request Entity Too Large"));
	status_line.insert(std::pair<int, std::string>(414, "URI Too Long"));
	status_line.insert(std::pair<int, std::string>(415, "Unsupported Media Type"));
	status_line.insert(std::pair<int, std::string>(416, "Range Not Satisfiable"));
	status_line.insert(std::pair<int, std::string>(417, "Expectation Failed"));

	status_line.insert(std::pair<int, std::string>(421, "Misdirected Request"));
	status_line.insert(std::pair<int, std::string>(422, "Unprocessable Entity"));
	status_line.insert(std::pair<int, std::string>(423, "Locked"));
	status_line.insert(std::pair<int, std::string>(424, "Failed Dependency	"));
	status_line.insert(std::pair<int, std::string>(425, "Too Early"));
	status_line.insert(std::pair<int, std::string>(426, "Upgrade Required"));
	status_line.insert(std::pair<int, std::string>(428, "Precondition Required"));
	status_line.insert(std::pair<int, std::string>(429, "Too Many Requests"));
	status_line.insert(std::pair<int, std::string>(431, "Request Header Fields Too Large"));
	status_line.insert(std::pair<int, std::string>(451, "Unavailable For Legal Reasons"));

	status_line.insert(std::pair<int, std::string>(418, "I’m a teapot"));
	status_line.insert(std::pair<int, std::string>(420, "Policy Not Fulfilled"));
	status_line.insert(std::pair<int, std::string>(444, "No Response"));
	status_line.insert(std::pair<int, std::string>(449, "The request should be retried after doing the appropriate action"));
	status_line.insert(std::pair<int, std::string>(499, "Client Closed Request"));

	status_line.insert(std::pair<int, std::string>(500, "Internal Server Error"));
	status_line.insert(std::pair<int, std::string>(501, "Not Implemented"));
	status_line.insert(std::pair<int, std::string>(502, "Bad Gateway"));
	status_line.insert(std::pair<int, std::string>(503, "Service Unavailable"));
	status_line.insert(std::pair<int, std::string>(504, "Gateway Timeout"));
	status_line.insert(std::pair<int, std::string>(505, "HTTP Version not supported"));
	status_line.insert(std::pair<int, std::string>(506, "Variant Also Negotiates"));
	status_line.insert(std::pair<int, std::string>(507, "Insufficient Storage"));
	status_line.insert(std::pair<int, std::string>(508, "Loop Detected"));
	status_line.insert(std::pair<int, std::string>(509, "Bandwidth Limit Exceeded"));
	status_line.insert(std::pair<int, std::string>(510, "Not Extended"));
	status_line.insert(std::pair<int, std::string>(511, "Network Authentication Required"));

	launch(data.getPorts());
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

    signal(SIGQUIT, shutdown);
	int len = 0;
	while (is_running)
	{
		struct timeval timeout;

		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		tmp_read_sockets = read_sockets;
		tmp_write_sockets = write_sockets;
		if ((len = select(FD_SETSIZE, &tmp_read_sockets, &tmp_write_sockets, NULL, &timeout)) < 0) // should a timer be set at 5th argument?
		{
			if (errno == EINTR)
				break;
			perror("Error");
			exit(EXIT_FAILURE);
		}
		std::cout << " select" << len << std::endl;
		for (int i = 0; len && is_running && i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &tmp_write_sockets)) // fd is ready to be written if true
			{
				tmp_socket_fd = i;
				responder();
				len--;
			}
			else if (FD_ISSET(i, &tmp_read_sockets)) // fd is ready to be read if true
			{
				tmp_socket_fd = i;
				handler();
				len--;
			}
			if(FD_ISSET(i, &read_sockets) && std::find(listeners.begin(), listeners.end(), i) == listeners.end())/* && tmp->second.get_parsing_position() == done_with_send*/
			{
				std::map<int, Request>::iterator itr = data.find(i);
				if(itr != data.end() && itr->second.get_parsing_position() == done_with_send)
				{
					int tmp_time = ft::ft_time_dif(itr->second.get_time_of_change());
					if (tmp_time > 2 || itr->second.get_method() != "GET")
					{
						std::cout << "time=" << tmp_time << "method" << itr->second.get_method()<<"." << std::endl;
						itr->second.wait_for_child();
						data.erase(itr);
						std::cout << "close socket summe = " << summe  << "fd=" << tmp_socket_fd << std::endl;
						FD_CLR(i, &read_sockets);
						close(i);
					}
				}
			}
		}
	}
	this->clear();
}

void SERVER::WebServer::handler()
{
	std::map<int,Request>::iterator tmp;

	if (std::find(listeners.begin(), listeners.end(), tmp_socket_fd) != listeners.end()) // check if it is in linsteners vector array
	{
		handle_new_client();
	}
	else // data from an existing connection, receive it
	{
		handle_known_client();
	}
}

void SERVER::WebServer::handle_new_client()
{
	accepter();
	if (tmp_socket_fd != -1)
	{
		fcntl(tmp_socket_fd, F_SETFL, O_NONBLOCK);
		FD_SET(tmp_socket_fd, &read_sockets);
		std::cout << "handle_new_client" << std::endl;
		Request req(*this);
		data.insert(std::pair<int, Request>(tmp_socket_fd, req));
	}
	std::cout << BOLD(FGRN("\n ---- ACCEPTED NEW ---- ")) << " fd " << tmp_socket_fd << std::endl;
}

void SERVER::WebServer::accepter()
{
	u_long addrlen = sizeof(struct sockaddr_storage);
	tmp_socket_fd = accept(tmp_socket_fd, (struct sockaddr *)&tmp_client_saddr, (socklen_t *)&addrlen); // copies the listen fd, the new fd is responsible for data transfer
}

void SERVER::WebServer::handle_known_client()
{
	std::map<int, Request>::iterator itr = data.find(tmp_socket_fd);
	if(itr != data.end())
	{
		if (itr->second.get_parsing_position() == done_with_send)
		{
			std::cout << "erase" << " fd=" << tmp_socket_fd << std::endl;
			data.erase(itr);
		}
	}
	itr = data.find(tmp_socket_fd);
	if(itr == data.end())
	{
		std::cout << "constructor" << std::endl;
		Request req(*this);
		data.insert(std::pair<int, Request>(tmp_socket_fd, req));
		itr = data.find(tmp_socket_fd);
	}
	std::cout << "process socket information" << std::endl;
	itr->second.fill_header(tmp_socket_fd, *config);
	if (itr->second.get_error_status())
	{
		perror(BOLD(FRED(" ---- ERROR ---- ")));
		// FD_CLR(tmp_socket_fd, &read_sockets);
	}
	if (itr->second.get_parsing_position() == send_first || itr->second.get_error_status())
	{
		std::cout << "set fd to write list fd" << tmp_socket_fd <<std::endl;
		if (!FD_ISSET(tmp_socket_fd, &write_sockets))
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
		if(info.get_parsing_position() <= send_first)
		{
			std::cout << "send first" << std::endl;
			Response response(info, *config);
			http_response = response.get_http_response(status_line);
			info.set_parsing_position(erase_cgi_header);
			// FD_CLR(tmp_socket_fd, &read_sockets);
		}
		else
		{
			std::cout << "send body" << std::endl;
			http_response = info.get_cgi_return();
			total = http_response.length();
			summe += total;
			std::stringstream stream;
			stream << std::hex << total;
			http_response.insert(0, std::string(stream.str() + "\r\n"));
			http_response.append("\r\n");
			// std::cerr << http_response;
			if(total == 0)
				end_of_chunked = true;
			stream.clear();
		}

		total = http_response.length();
		// std::cout << std::string(http_response, 0, total) << std::endl;
		const char *ptr = static_cast<const char *>(http_response.c_str());
		while (total > 0)
		{
			int bytes = send(tmp_socket_fd, static_cast<const void *>(ptr), total, 0);
			if (bytes != total)
				std::cout << "error send " << bytes << "should" << total<< "measage" << http_response << std::endl;
			if (bytes == -1 || bytes == 0)
			{
				std::cout << "break" << std::endl;
				break ; //HTTP server closes the socket if an error occurs during the sending of a file
			}
			else
			{
				ptr += bytes;
				total -= bytes;
			}
		}
		if(/*(info.get_method() != "POST" ) || */end_of_chunked || !info.is_chunked())
		{
			std::cout << "end of response" << std::endl;
			summe = 0;
			FD_CLR(tmp_socket_fd, &write_sockets);
			info.set_parsing_position(done_with_send);
			// data.erase(itr);
			// close(tmp_socket_fd);
		}
		std::cout << BOLD(FBLU(" ---- RESPONDED ---- ")) << "fd " << tmp_socket_fd << std::endl;
	}
}

void SERVER::WebServer::close_all_pipes()
{
	for (std::map<int,Request>::iterator itr = data.begin(); itr != data.end(); itr++)
	{
		itr->second.close_pipe_in();
	}
}

void SERVER::WebServer::clear()
{
	std::map<int, Request>::iterator itr = data.begin();
    while (itr != data.cend())
    {
		int fd = itr->first;
		if (FD_ISSET(fd, &write_sockets))
		{
			FD_CLR(fd, &write_sockets);
		}
		if (FD_ISSET(fd, &read_sockets))
		{
			FD_CLR(fd, &read_sockets);
		}
		close(fd);
    }
	for (std::vector<int>::iterator it = listeners.begin(); it != listeners.end(); it++)
	{
		int fd = itr->first;

		if (FD_ISSET(fd, &write_sockets))
		{
			FD_CLR(fd, &write_sockets);
		}
		if (FD_ISSET(fd, &read_sockets))
		{
			FD_CLR(fd, &read_sockets);
		}
		close(fd);
	}
	data.clear();
	listeners.clear();
	delete config;
	std::cout << "\n" << BOLD(FGRN(" ---- SERVER IS SHUTTING DOWN ---- ")) << std::endl;
}

void	SERVER::WebServer::shutdown(int signal)
{
	(void)signal;
	std::cout << "sigquit" << std::endl;
	std::cerr << std::flush;
	is_running = false;
}
