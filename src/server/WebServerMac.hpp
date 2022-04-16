#pragma once
#ifndef WebServer_HPP
#define WebServer_HPP

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <map>
#include <ctime>
#include <sys/stat.h>
#include "SimpleServer.hpp"
#include <algorithm>
#include "../../inc/Colors.hpp"
#include "../../inc/Configuration.hpp"
#include "../../inc/Parsing.hpp"

class Request;

namespace SERVER
{
	class WebServer: public SimpleServer
	{
		private:
			struct sockaddr_storage			tmp_client_saddr;
			std::vector<int>				listeners;				//vector of file descripters of the new created listener sockets
			int								tmp_socket_fd;
			fd_set							read_sockets;
			fd_set							write_sockets;
			std::map<int,Request>			data;
			std::map<int,std::string>		status_line;
			Config*							config;
			struct sigaction				sigact;

			void							accepter();
			void							handler();
			void							responder();
			void							clear();
			void							handle_new_client();
			void							handle_known_client();
			void							respond_header(std::stringstream& client, Request& info);
			void							respond_body(std::stringstream& client, Request& info);
			std::string						http_time(const struct tm *timeptr);
			static bool						is_running;
			static void						shutdown(int a);

		public:
			void							close_all_pipes();
			WebServer(Config& config);
			void launch(std::vector <int> &ports);
			~WebServer();
	};


}

#endif



