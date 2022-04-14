#ifndef SimpleSocket_HPP
#define SimpleSocket_HPP

#include <netinet/in.h> // For sockaddr_in

namespace SOCKET
{
	class SimpleSocket
	{
		private:
			int socket_fd;
			struct sockaddr_in addr;

		public:
			SimpleSocket(int domain, int type, int protocol, int port, u_long interface);
			void check_return(int value);
			~SimpleSocket();
			int get_socket_fd();
			sockaddr_in get_addr();
	};
	
}

#endif
