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
#include "../../inc/Parsing.hpp"
#include "../../inc/Configuration.hpp"

namespace SERVER
{
	class WebServer: public SimpleServer
	{
	private:
		struct sockaddr_storage tmp_client_saddr;
		std::vector<int>		listeners;				//vector of filedescriptes of the new created lisner sockets
		int						tmp_socket_fd;
		fd_set					current_sockets;
		fd_set					write_sockets;
		std::map<int,Parsing>	data;
		Config					config;
		void					accepter();
		void					handler();
		void					responder();
		void					handle_new_client();
		void					handle_known_client();
		void					respond_header(std::stringstream& client, Parsing& info);
		void					respond_body(std::stringstream& client, Parsing& info);
		std::string				http_time(const struct tm *timeptr);

		LocationData * test(std::string name);
		int get_truncated_location(std::vector<std::string>, std::string);

	public:
		WebServer(std::vector<int>& ports, Config config);
		void launch(std::vector <int> &ports);
		~WebServer();
	};


}
#endif