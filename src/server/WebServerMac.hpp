#ifndef WebServer_HPP
#define WebServer_HPP

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h> 
#include <map>
#include <ctime>
#include "SimpleServer.hpp"
#include "../../inc/Parsing.hpp"

#define MAX_EVENTS                10

namespace SERVER
{
	class WebServer: public SimpleServer
	{
	private:
		struct sockaddr_storage tmp_client_saddr;
		int						tmp_socket_fd;
		char					buffer[1500];
		fd_set					current_sockets;
		fd_set					tmp_write_sockets;
		std::map<int,Parsing>	data;
		void					accepter();
		void					handler();
		void					responder();
		void					handle_new_client();
		void					handle_known_client();
		void					respond_header();

	public:
		WebServer(int domain, int type, int protocol, int port, u_long interface, int backlog);
		void launch();
		~WebServer();
	};
	

}
#endif