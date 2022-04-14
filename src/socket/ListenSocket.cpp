#include "ListenSocket.hpp"

SOCKET::ListenSocket::ListenSocket(int domain, int type, int protocol, int port, u_long interface, int backlog): BindingSocket(domain, type, protocol, port, interface)
{
	check_return(listen(get_socket_fd(), backlog));	
}
	
SOCKET::ListenSocket::~ListenSocket() {}
