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

class Parsing
{
	private:
        std::map<std::string, std::string>  headers;
        std::string                         method;
        std::string                         path;
        std::string                         protocol;
        std::vector<std::string>            body;

	public:
        Parsing (char *buffer);
        void    for_testing_print_request_struct();
        int     get_body(std::string line);
        int     get_headers(std::string line);
        int     get_start_line(std::string s);

};

// typedef struct s_request
// {
//     std::map<std::string, std::string> headers;
//     std::string method;
//     std::string path;
//     std::string protocol;
//     std::vector<std::string> body;
// } t_request;

// int parse_request(t_request *request, int fd, char *buffer);
// void for_testing_print_request_struct(t_request *request);
// int get_body(std::string line, t_request *request);
// int get_headers(std::string line, t_request *request);
int is_whitespace(std::string line);
// int get_start_line(std::string s, t_request *request);

#endif
