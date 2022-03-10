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
       
        private:
        void    for_testing_print_request_struct();
        int     get_body(std::string line);
        int     get_headers(std::string line);
        int     get_start_line(std::string s);

};

int is_whitespace(std::string line);


#endif
