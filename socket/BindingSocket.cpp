	
	#include "BindingSocket.hpp"
	
	SOCKET::BindingSocket::BindingSocket(int domain, int type, int protocol, int port, u_long interface): SimpleSocket(domain, type, protocol, port, interface)
	{
		sockaddr_in address = get_addr();
		check_return(bind(get_socket_fd(), (struct sockaddr *)&address, sizeof(address)));
	}
	
	SOCKET::BindingSocket::~BindingSocket()
	{
	}
	