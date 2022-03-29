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
#include <cstdlib>	  // For exit() and EXIT_FAILURE
#include <unistd.h>	 // For read
#include <sstream>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "../../inc/Helper.hpp"
#include "../../inc/Configuration.hpp"

class Response
{
private:
		bool is_path_valid;
		bool has_access;
		Request request;
		Config config;
		std::string default_error;
		std::ifstream input_stream;
		std::string body;
		std::string status_line;
		std::string path;
		std::string file_ext;
		std::map<std::string, std::string> headers;

		void set_body(void);
		void set_headers(void);
		void set_path(std::string const filename);
		void set_error_path(void);
		void set_status_line(void);
		void set_content_type(void);
		void set_image_body(void);
public:
		std::string get_path(void);
		Response(Request req, Config data);
		std::string get_http_response(void);
		std::string get_http_time();
		void	stop_reading(void);
};

bool is_image_ext(std::string ext);
bool is_text_ext(std::string ext);
bool is_authorized(std::string server, std::string request, bool listing_status);
bool exists_path(std::string const filename);

#endif
