#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
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
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stddef.h>

#include "../../inc/Configuration.hpp"
#include "../../inc/Http_header.hpp"
#include "../../inc/Helper.hpp"

class Request;

class Response : public http_header_response
{
	private:
			Request			&request;
			Config			*config;
			std::string		default_error;
			std::ifstream	input_stream;
			std::string		body;
			std::string		path;
			std::string		default_err;
			std::string		file_ext;
			void set_body(void);
			void set_headers(void);
			void set_path(std::string const filename);
			void set_error_path(void);
			void set_content_type(void);
			void set_image_body(void);
			void set_text_body(void);
			void set_php_body(void);
			void set_error_page(std::string file);
			void stop_writing(void);
			void handle_delete_request(void);
	public:
			Response(Request& req, Config& data);

			std::string get_path(void);
			std::string get_http_response(std::map<int,std::string>& status_line);
			std::string get_http_time();
};

bool is_redirection(std::string path);
bool is_image_ext(std::string ext);
bool is_text_ext(std::string ext);
bool is_authorized(std::string server, std::string request, bool listing_status);
bool is_method_valid(std::string method);
bool is_page_not_found(std::string path, std::string root);
bool exists_path(std::string const filename);
bool exists_dir(std::string const path);

#endif
