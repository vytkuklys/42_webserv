#ifndef Parsing_HPP
# define Parsing_HPP

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
#include "../../inc/Helper.hpp"
#include <fstream>	  // std::ifstream

enum mile_stones{
	first_line,
	header,
	first_body,
	body
};

class Parsing
{
	private:
		std::map<std::string, std::string>	headers;
		std::string							method;
		std::string							path;
		std::string							protocol;
		std::string							body; //not nessesary ?
		int									pipe_in[2];
		unsigned long						missing_chuncked_data;
		std::string							part_of_hex_of_chunked;
		pid_t								pid_child;
		mile_stones							parsing_position;
		std::string							raw_header_line;
		std::string							port;  //not nessesary ?
		std::string							status_line;

	public:
		std::string get_body() const;   //not nessesary ?
		Parsing (int fd);
		~Parsing();
		std::string get(std::string key_word);
		std::map<std::string, std::string> get_header() const;
		std::string		get_method() const;
		std::string		get_path() const;
		std::string		get_protocol() const;
        std::string		get_port();
		std::string		get_status_line() const;
		int				get_parsing_position() const;
		void			set_status_line(std::string new_status);
		int				get_content_length();
		bool			is_chunked(void);
		void			set_regular_body(int fd);
		void			set_chunked_body(int fd);
		void			fill_header(int fd);
	private:
		void	for_testing_print_request_struct();
		int		set_headers(std::string line);
		bool		set_start_line(std::string s);
		void	set_regular_body(std::istringstream& data, int bytes); //not nessesary ?
		void	set_chunked_body(std::istringstream& data, int bytes); //not nessesary ?
		void	unchunk_body(std::istringstream& data);
};

#endif
