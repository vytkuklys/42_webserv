#ifndef BindingSocket_HPP
#define BindingSocket_HPP

#include "SimpleSocket.hpp"

namespace SOCKET
{
	class BindingSocket: public SimpleSocket
	{
		private:

		public:
			BindingSocket(int domain, int type, int protocol, int port, u_long interface);
			~BindingSocket();
	};

}

#endif
