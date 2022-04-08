#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <sys/socket.h> // For socket functions
#include <cstdlib>	  // For exit() and EXIT_FAILURE
#include <unistd.h>	 // For read
#include <sstream>
#include <stdio.h>
#include <fcntl.h>
#include <algorithm>
#include <cstring>
#include "../../inc/Http_header.hpp"
#include "../../inc/Helper.hpp"
#include "../../inc/Configuration.hpp"
#include <fstream>	  // std::ifstream
// #include "Response.hpp"
#include <sys/wait.h>
#include <strings.h>

#define NOT_SET 0

enum mile_stones{
	first_line,
	header,
	done_with_header,
	first_chunk_size,
	body,
	send_first,
	erase_cgi_header,
	send_body
};

class Request : public http_header_request
{
	private:
		int									pipe_in[2];
		FILE*								out_file;
		int									pipe_out[2];
		int									content_length;
		unsigned long						missing_chuncked_data;
		std::string							part_of_hex_of_chunked;
		pid_t								pid_child;
		mile_stones							parsing_position;
		std::string							raw_header_line;
		std::string							status_line;
		bool								is_error;
		Config								*config;
		int									remove_n;
		int 								max_body;

	public:
		Request (Config& conf);
		~Request();

		std::string get(std::string key_word);
		std::map<std::string, std::string> get_header() const;
		// std::string		get_method() const;
		// std::string		get_path() const;
		// std::string		get_protocol() const;
        // std::string		get_port();
		std::string		get_status_line() const;
		int				get_parsing_position() const;
		int				get_content_length();
		bool			get_error_status() const;
		std::string		get_cgi_return();

		void			set_status_line(std::string new_status);
		void			set_regular_body(std::istringstream& data);
		void			set_chunked_body(int fd);
		void			set_parsing_position(mile_stones new_pos);

		void			fill_header(int fd);
		bool			is_chunked(void);

	private:
		void	for_testing_print_request_struct();
		bool	is_content_length_valid();

		int		set_headers(std::string line);
		bool	set_start_line(std::string s);
		void	set_error_status(bool status);

		void	unchunk_body(std::istringstream& data);

		void	stop_reading(std::string status, bool close_fd);
		void	set_max_body(void);
};

#endif
