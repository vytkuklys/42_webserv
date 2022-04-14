#ifndef ListenSocket_HPP
#define ListenSocket_HPP

#include "BindingSocket.hpp"

namespace SOCKET
{
	class ListenSocket: public BindingSocket
	{
		private:

		public:
			ListenSocket(int domain, int type, int protocol, int port, u_long interface, int backlog);
			~ListenSocket();
	};
}

#endif
