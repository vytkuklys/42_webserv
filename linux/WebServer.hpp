#ifndef WebServer_HPP
#define WebServer_HPP

// #include <sys/epoll.h>
#include <sys/socket.h>
#include "SimpleServer.hpp"
#include "../../inc/Parsing.hpp"

#define MAX_EVENTS                10

namespace SERVER
{
	class WebServer: public SimpleServer
	{
	private:
		int						efd;
		struct sockaddr_storage tmp_client_saddr;
		int						tmp_socket_fd;
		char					buffer[1500];
		struct epoll_event		ev;
		void					accepter();
		void					handler();
		void					responder();
		void					handle_new_client();
		void					handle_known_client();

	public:
		WebServer(int domain, int type, int protocol, int port, u_long interface, int backlog);
		void launch();
		~WebServer();
	};
	

}
#endif