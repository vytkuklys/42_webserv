#ifndef SimpleServer_hpp
#define SimpleServer_hpp

#include "../socket/ListenSocket.hpp"

namespace SERVER
{
	class SimpleServer
	{
	private:
		SOCKET::ListenSocket* socket;
		virtual void accepter() = 0;
		virtual void handler() = 0;
		virtual void responder() = 0;
	public:
		SimpleServer(int domain, int type, int protocol, int port, u_long interface, int backlog);
		~SimpleServer();
		virtual void launch() = 0;
		SOCKET::ListenSocket* get_socket();

	};
		
}

#endif