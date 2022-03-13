#include<iostream>
#include "SimpleSocket.hpp"
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

SOCKET::SimpleSocket::SimpleSocket(int domain, int type, int protocol, int port, u_long interface)
{
	addr.sin_family = domain;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(interface);
	socket_fd = socket(domain, type, protocol);
	check_return(socket_fd);
	int optval = 1;
	if (setsockopt (socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (int)) == -1)
		perror("csetsockopt");

}

SOCKET::SimpleSocket::~SimpleSocket()
{
	close(socket_fd);
}

void SOCKET::SimpleSocket::check_return(int value)
{
	if(value < 0)
	{
		perror("can not creat socket");
		return ;
	}
}

int SOCKET::SimpleSocket::get_socket_fd()
{
	return(socket_fd);
}

sockaddr_in SOCKET::SimpleSocket::get_addr()
{
	return(addr);
}