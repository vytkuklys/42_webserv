#include "SimpleServer.hpp"
#include <iostream>

SERVER::SimpleServer::SimpleServer(int domain, int type, int protocol, std::vector<int>& ports, u_long interface, int backlog)
{
	// std::cout << "test" << ports.back() <<std::endl;
	for (std::vector<int>::iterator port = ports.begin(); port != ports.end(); port++)
	{
		SOCKET::ListenSocket* tmp = new SOCKET::ListenSocket(domain, type, protocol, *port, interface, backlog);
		sockets.push_back(tmp);
		// std::cout << "loop" << sockets.back()->get_socket_fd() << std::endl;
	}
}

SERVER::SimpleServer::~SimpleServer()
{
	for(std::vector<SOCKET::ListenSocket*>::reverse_iterator socket = sockets.rbegin(); socket != sockets.rend(); ++socket)
	{
		delete((*socket));
		sockets.pop_back();
		// std::cout << "test delete" << std::endl;

	}
}

std::vector<SOCKET::ListenSocket*>& SERVER::SimpleServer::get_sockets()
{
	return(sockets);
}