#pragma once
#ifndef SimpleServer_HPP
#define SimpleServer_HPP
#include "../socket/ListenSocket.hpp"
#include <vector>


namespace SERVER
{
	class SimpleServer
	{
		private:
			std::vector<SOCKET::ListenSocket*> sockets;
			virtual void accepter() = 0;
			virtual void handler() = 0;
			virtual void responder() = 0;

		public:
			SimpleServer(int domain, int type, int protocol, std::vector<int>& ports, u_long interface, int backlog);
			~SimpleServer();
			virtual void launch(std::vector <int> &ports) = 0;
			std::vector<SOCKET::ListenSocket*>& get_sockets();
	};
}

#endif
