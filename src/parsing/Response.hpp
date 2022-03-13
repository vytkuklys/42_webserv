#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
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
#include "../../inc/Helper.hpp"

class Response
{
private:
        bool is_path_valid;
        Parsing Request;
        std::ifstream input_stream;
        std::string body;
        std::string status_line;
        std::string path;
        std::map<std::string, std::string> headers;

        void set_body(void);
        void set_headers(void);
        void set_path(std::string const filename);
        void set_status_line(void);
        std::string get_path(void);

public:
        Response(Parsing request);
        std::string get_http_response(void);
        std::string get_http_time();
};

bool exists_path(std::string const filename);

#endif
