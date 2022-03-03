#ifndef ListenSocket_hpp
#define ListenSocket_hpp

#include "BindingSocket.hpp"

namespace SOCKET
{
	class ListenSocket: public BindingSocket
	{
	private:
		/* data */
	public:
		ListenSocket(int domain, int type, int protocol, int port, u_long interface, int backlog);
		~ListenSocket();
	};

	
}

#endif