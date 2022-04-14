#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <sys/socket.h>
#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <stdio.h>
#include <fcntl.h>
#include <algorithm>
#include <cstring>
#include "../../inc/Http_header.hpp"
#include "../../inc/Helper.hpp"
#include "../../inc/Configuration.hpp"
#include "../../inc/Colors.hpp"
#include "../../inc/Server.hpp"
#include <fstream>
#include <sys/wait.h>
#include <strings.h>

#define NOT_SET 0

enum mile_stones{
	read_first_line,
	read_header,
	done_with_header,
	read_first_chunk_size,
	body,
	send_first,
	erase_cgi_header,
	send_body,
	done_with_send
};

namespace SERVER
{
	class WebServer;
}

class Request : public http_header_request
{
	private:
		SERVER::WebServer					&webserver;
		int									pipe_in[2];
		FILE*								out_file;
		int									content_length;
		unsigned long						missing_chuncked_data;
		std::string							part_of_hex_of_chunked;
		pid_t								pid_child;
		mile_stones							parsing_position;
		std::string							raw_header_line;
		bool								is_error;
		bool								is_pipe_open;
		bool								is_forked;
		LocationData						*config;
		int									remove_n;
		int 								max_body;
		int									chunked_size;
		int									count_read_byts_from_file;
		unsigned long						time_of_change;
		int									status_code;

	public:
		Request(SERVER::WebServer &tmp_webserver);
		~Request();

		std::map<std::string, std::string> get_header() const;
		unsigned long	get_time_of_change();
		int				get_status_code() const;
		int				get_parsing_position() const;
		int				get_content_length();
		bool			get_error_status() const;
		std::string		get_cgi_return();
		void			close_pipe_in();

		void			set_status_code(int code);
		void			set_regular_body(std::istringstream& data);
		void			set_chunked_body(int fd);
		void			set_parsing_position(mile_stones new_pos);

		void			fill_header(int fd, Config& conf);
		bool			is_chunked(void);
		void			wait_for_child();

	private:
		void			for_testing_print_request_struct();
		bool			is_payload_too_large();
		bool			is_chunked_payload_too_large();

		int				set_headers(std::string line);
		bool			set_start_line(std::string s);
		void			set_error_status(bool status);

		void			unchunk_body(std::istringstream& data);

		void			stop_reading(int code);
		void			set_max_body();
		void			set_up_cgi_proces();
		void			set_up_child();
		bool			proces_chunked_size(std::string& line);
		bool			proces_chunked_body(std::istringstream& data);
};

#endif
