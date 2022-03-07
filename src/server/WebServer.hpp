#ifndef WebServer_hpp
#define WebServer_hpp

#include "SimpleServer.hpp"
#include "../../inc/Parsing.hpp"
#include <sys/epoll.h>

namespace SERVER
{
	class WebServer: public SimpleServer
	{
	private:
		Parsing* request;
		int new_socket;
		char buffer[1500];
		void accepter();
		void handler();
		void responder();
	public:
		WebServer(int domain, int type, int protocol, int port, u_long interface, int backlog);
		void launch();
		~WebServer();
	};
	

}
#endif