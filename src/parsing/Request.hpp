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

class Parsing
{
	private:
		std::map<std::string, std::string>  headers;
		std::string						 method;
		std::string						 path;
		std::string						 protocol;
		std::string						 body;
		int								 pipefd[2];
		unsigned long					 missing_chuncked_data;
		pid_t							pid_child;
	public:
		Parsing (int fd);
		std::string get(std::string key_word);
		std::map<std::string, std::string> get_header() const;
		std::string get_method() const;
		std::string get_path() const;
		std::string get_protocol() const;
		std::string get_body() const;
		bool is_chunked(void);
		void	set_regular_body(int fd);
		void	set_chunked_body(int fd);
		private:
		void	for_testing_print_request_struct();
		int	 set_headers(std::string line);
		int	 set_start_line(std::string s);
		void	set_regular_body(std::istringstream& data, int bytes);
		void	set_chunked_body(std::istringstream& data, int bytes);
		int	 get_content_length();
		void unchunk_body(std::istringstream& data);
};

#endif
