#ifndef Parsing_HPP
# define Parsing_HPP

#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <sys/socket.h> // For socket functions
#include <cstdlib>      // For exit() and EXIT_FAILURE
#include <unistd.h>     // For read
#include <sstream>
#include <stdio.h>
#include <fcntl.h>
#include <algorithm> 

class Parsing
{
	private:
        std::map<std::string, std::string>  headers;
        std::string                         method;
        std::string                         path;
        std::string                         protocol;
        std::string                         body;

	public:
        Parsing (int fd);
        std::string get(std::string key_word);
        std::map<std::string, std::string> get_header() const;
        std::string get_method() const;
        std::string get_path() const;
        std::string get_protocol() const;
        std::string get_body() const;

        private:
        void    for_testing_print_request_struct();
        int     set_headers(std::string line);
        int     set_start_line(std::string s);
};

int is_whitespace(std::string line);


#endif
