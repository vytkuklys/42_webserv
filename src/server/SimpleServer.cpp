#include "SimpleServer.hpp"

SERVER::SimpleServer::SimpleServer(int domain, int type, int protocol, int port, u_long interface, int backlog)
{
	socket = new SOCKET::ListenSocket(domain, type, protocol, port, interface, backlog);
}

SERVER::SimpleServer::~SimpleServer()
{
	delete(socket);
}

SOCKET::ListenSocket* SERVER::SimpleServer::get_socket()
{
	return(socket);
}