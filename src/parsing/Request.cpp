#include "Request.hpp"
#include "cstring"
bool Request::set_start_line(std::string s)
{
	size_t pos = 0;
	std::string token;
	int j = 0;
	if(s.find("\r") == std::string::npos)
	{
		raw_header_line += s;
		return (EXIT_FAILURE);
	}
	parsing_position = header;
	if (raw_header_line.empty() == false)
	{
		s = raw_header_line + s;
		raw_header_line.clear();
	}
	s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
	if (s[0] == '/' || s[0] == '#')
		return (EXIT_FAILURE);
	// std::cout << "set_sart_line" << std::endl;
	while ((pos = s.find(" ")) != std::string::npos)
	{
		token = s.substr(0, pos);
		if ((token.length()) > 0 && j == 0)
		{
	// std::cout << "set_sart_line" << std::endl;
			set_method(token);
	// std::cout << "set_sart_line" << std::endl;
			j++;
		}
		else if (token.length() > 0 && j++ == 1)
			set_path(token);
		s.erase(0, pos + 1);
	}
	set_protocol(s);
	return (EXIT_SUCCESS);
}

int Request::set_headers(std::string line)
{
	size_t pos;
	if(line.find("\r") == std::string::npos)
	{
		raw_header_line += line;
		return (EXIT_FAILURE);
	}
	if (raw_header_line.empty() == false)
	{
		line = raw_header_line + line;
		raw_header_line.clear();
	}
	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); //remove the newlines
	if (line.empty() || ft::is_whitespace(line) == EXIT_SUCCESS)
	{
		parsing_position = done_with_header;
		return (EXIT_FAILURE);
	}
	pos = line.find(":");
	if (!(line[0] == '/' || line[0] == '#'))
		set_value(line.substr(0, pos), line.substr(pos + 2, line.length()));
	return (EXIT_SUCCESS);
}

void Request::for_testing_print_request_struct()
{
	std::cout << "\n---START LINE \n";

	std::cout << get_http_header() << std::endl;
}

Request::Request(Config& conf)
{
	parsing_position = first_line;
	missing_chuncked_data = 0;
	max_body = NOT_SET;
	set_error_status(false);
	config = &conf;
	status_line = "HTTP/1.1 200 OK";
	remove_n = false;
}

Request::~Request()
{
	return;
}

void	Request::fill_header(int fd)
{
	char buffer[4001];
	std::string line;
	size_t bytes = recv(fd, buffer, 4000, 0);
	if (static_cast<int>(bytes) == -1)
	{
		stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", false);
		return ;
	}
	else if (static_cast<int>(bytes) == 0)
	{
		stop_reading("HTTP/1.1 400 BAD REQUEST", false);
		return ;
	}
	// std::cout << "bysts=" << bytes << "\nbuffer\n" << buffer << std::endl;
	// std::cout << "==============buffer====================" <<std::endl;
	// write(1, buffer, bytes);
	// std::cout << "==============buffer====================" <<std::endl;
	std::istringstream data(std::string(buffer, bytes), std::ios::binary);
	// std::cout << "missing_dat: " << "." << data.str().length() << "." << std::endl;
	std::cout << "fill header" << std::endl;

	while ((parsing_position == first_line) && data && std::getline(data, line))
	{
		if(line.empty())
			continue;
		if (set_start_line(line) == EXIT_SUCCESS)
			break;
	}
	std::cout << "fill header" << std::endl;

	std::cout << "date=" << data << "bytes" << bytes << std::endl;
	while ((parsing_position == header) && data && std::getline(data, line))
	{
		if(line.empty())
			continue;
		if (set_headers(line) == EXIT_FAILURE)
			break;
	}

	if (parsing_position == done_with_header)
	{
		content_length = get_content_length();
		if (max_body == NOT_SET)
		{
			set_max_body();
			if (is_content_length_valid() == false)
			{
				stop_reading("HTTP/1.1 413 PAYLOAD TOO LARGE", false);
				return ;
			}
		}
		// std::cout << "fd = " << fd << std::endl;
		// for_testing_print_request_struct();
		if (get_method() == "POST" || get_method() == "PUT") // check if it is a post request
		{
			if (content_length != 0)
			{
				if (pipe(pipe_in) == -1)
				{
					stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", false);
					return ;
				}
				out_file = tmpfile();
				// if (pipe(pipe_out) == -1)
				// {
				// 	close(pipe_in[0]);
				// 	close(pipe_in[1]);
				// 	stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", false);
				// 	return ;
				// }
				std::cout << "fork" <<std::endl;
				pid_child = fork();
				if (pid_child == -1)
				{
					stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", false);
					return ;
				}
				else if (pid_child == 0)
				{
					std::vector<char *> env;
					std::vector<char *> argv;
					std::vector<std::string> env_strings;
					std::vector<std::string> argv_strings;
					argv_strings.resize(2);
					argv_strings.push_back("cgi_tester");
					argv_strings.push_back(get_path());
					// argv_strings.push_back(config->get_location(get_port(), get_path())->getScript());
					std::cout << "execve" << std::endl;
					for(size_t i = 0; i < argv_strings.size(); i++)
					{
						argv.push_back(&(argv_strings[i][0]));
					}
					argv.push_back(NULL);
					std::string tmp;
					env_strings.resize(25);
					// if ((tmp = get_value("Content-Length")) != "not found")
					// {
					// 	env_strings.push_back("CONTENT_LENGTH=" + tmp);
					// }
					env_strings.push_back("PATH_INFO=" + get_path());
					env_strings.push_back("QUERY_STRING=");
					// env_strings.push_back("REQUEST_URI=" + get_path());
					env_strings.push_back("REDIRECT_STATUS=200");
					for(http_header::iterator tmp = http_header::begin(); tmp != http_header::end(); tmp++)
					{
						std::string key = tmp->first;
						std::transform(key.begin(), key.end(),key.begin(), ::toupper);
						for(std::string::iterator pos = key.begin(); pos != key.end(); pos++)
						{
							if(*pos == '-')
								*pos = '_';
						}
						env_strings.push_back("HTTP_" + key + "=" + tmp->second);
						std::cout << env_strings.back() << std::endl;
					}
					// env_strings.push_back("SCRIPT_NAME=" + config->get_location(get_port(), get_path())->getScript());
					// std::cout << "SCRIPT_FILENAME=." + get_path() << " = " << "SCRIPT_NAME=" + config->get_location(get_port(), get_path())->getScript() << std::endl;
					// if(get_method() == "POST")
					// 	env_strings.push_back("SCRIPT_FILENAME=." + get_path());
					// else
					// env_strings.push_back("SCRIPT_FILENAME=./cgi-bin/cgi.php");
					// std::cout << env_strings.back() << std::endl;
					// env_strings.push_back("DOCUMENT_ROOT=");
					env_strings.push_back("REQUEST_METHOD=" + get_method());
					env_strings.push_back("SERVER_PROTOCOL=" + get_protocol());
					env_strings.push_back("SERVER_SOFTWARE=webserv");
					env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");
					env_strings.push_back("REQUEST_SCHEME=http");
					if ((tmp = get_value("Host")) != "not found")
					{
						env_strings.push_back("SERVER_PORT=" + tmp);
						env_strings.back().erase(std::strlen("SERVER_PORT=") - 1, env_strings.back().find(":"));
					}
					env_strings.push_back("SERVER_ADDR=");
					if ((tmp = get_value("Host")) != "not found")
					{
						env_strings.push_back("SERVER_PORT=" + tmp);
						env_strings.back().erase(env_strings.back().find(":") - 1, env_strings.back().length() - 1);
					}

					if ((tmp = get_value("Content-Type")) != "not found")
					{
						env_strings.push_back("CONTENT_TYPE=" + tmp);
					}

					for(size_t i = 0; i < env_strings.size(); i++)
					{
						env.push_back(&(env_strings[i][0]));
					}
					env.push_back(NULL);
					close(pipe_in[1]);
					// close(pipe_out[0]);
					dup2(pipe_in[0], STDIN_FILENO); //stop reading
					close(pipe_in[0]);
					dup2(fileno(out_file), STDOUT_FILENO); //stop reading
					// close(pipe_out[1]);
					// char * const * nll = NULL;
					// chdir("./cgi-bin");
					if (execve(/*config->get_location(get_port(), get_path())->getScript().c_str()*/ "/Users/shackbei/Documents/code/Projects/webserv/cgi-bin/cgi_tester" , &argv[0], &env[0]))
					{
						std::cout << "script=" << config->get_location(get_port(), get_path())->getScript().c_str() << std::endl;
						perror("execve"); //stop reading
						exit(EXIT_FAILURE);
					}
					std::cout  << "script=" << config->get_location(get_port(), get_path())->getScript().c_str() << std::endl;
					perror("execve"); //stop reading
					exit(EXIT_FAILURE);
				}
				close(pipe_in[0]);
				// close(pipe_out[1]);
			}
		}
		else
		{
				parsing_position = send_first;
		}
	}
	if (parsing_position >= done_with_header && parsing_position != send_first)
	{
		if (is_chunked())
			unchunk_body(data);
		else
			set_regular_body(data);
	}
	std::cout << " parsing_position= " << parsing_position << std::endl;
}
// ----------------- SETTER ------------------ //

void	Request::set_status_line(std::string new_status)
{
	status_line = new_status;
}

void	Request::set_error_status(bool status)
{
	is_error = status;
}

void	Request::set_parsing_position(mile_stones new_pos)
{
	parsing_position = new_pos;
}

void Request::set_max_body(void)
{
	if (max_body == 0)
	{
		LocationData * loc = config->get_location(get_port(), get_path());
		if (loc == nullptr)
		{
			max_body = 1;
		}
		else
		{
			max_body = loc->getMaxBody();
		}
	}
}


// ----------------- GETTERS ------------------ //

// std::string Request::get(std::string key_word)
// {
// 	std::map<std::string, std::string>::iterator itr;
// 	if (key_word == "method")
// 		return (method);
// 	else if (key_word == "path")
// 		return (path);
// 	else if (key_word == "protocol")
// 		return (protocol);
// 	else
// 	{
// 		itr = headers.find(key_word);
// 		if (itr == headers.end())
// 			return (key_word);
// 		else
// 			return (std::string());
// 	}
// }

// std::map<std::string, std::string> Request::get_header() const
// {
// 	return (headers);
// }
// std::string Request::get_method() const
// {
// 	return (method);
// }

bool Request::get_error_status() const
{
	return (is_error);
}

// std::string Request::get_path() const
// {
// 	return (path);
// }
// std::string Request::get_protocol() const
// {
// 	return (protocol);
// }

// std::string Request::get_port()
// {
// 	std::string port;
//     if (headers.find("Host") != headers.end())
//     {
//         std::string host = headers.find("Host")->second;
//         port = host.substr(host.find_last_of(':') + 1, host.length());
//     }
//     return (port);
// }

int Request::get_content_length()
{
	// std::map<std::string, std::string>::iterator tmp = headers.find("Content-Length");
	// if (tmp != headers.end())
	// 	return (ft::stoi(tmp->second));
	// return (-1);
	std::string tmp = get_value("Content-Length");
	if(tmp == "not found")
		return (-1);
	return (ft::stoi(tmp));
}

std::string		Request::get_status_line() const
{
	return(status_line);
}

int		Request::get_parsing_position() const
{
	return(parsing_position);
}

std::string		Request::get_cgi_return()
{
	char tmp[4097];
	int bytes;
	if ((bytes = fread(tmp, 1, 4096, out_file)) < 4096)
	{
		std::cout << "close out_file" << std::endl;
		fclose(out_file);
		// close(pipe_out[0]);
	}
	if (parsing_position == erase_cgi_header)
	{
		std::string tmp1(tmp, bytes);
		std::cout << "remove header" << tmp1 << std::endl;
		parsing_position = send_body;
		return(tmp1.erase(0, tmp1.find("\r\n\r\n") + 4));
	}
	// if (std::string(tmp, bytes).find("\n") != std::string::npos)
	// 	std::cerr << std::string(tmp, bytes);
	return(std::string(tmp, bytes));
}



// --------------------- CHECKERS ---------------------

bool Request::is_chunked(void)
{
    if ((get_method() == "POST" || get_method() == "PUT") && get_content_length() == -1)
    {
        // std::cout << "Chunked branch\n";
        // std::cout << "down\n";
        // for_testing_print_request_struct();
        // std::cout << "\nup\n";
        // std::cout << "~~For debugging purposes.";
        //exit(1);
		std::cout << "is chunked" << std::endl;
        return (true);
    }
    return (false);
}

// --------------- OVERLOADS ---------------- //

void Request::set_regular_body(std::istringstream& data)
{
	char buffer[4001];
	std::cout << ",Len: " << content_length;
	size_t bytes = data.rdbuf()->in_avail();
	bytes = data.readsome(buffer, bytes);
	if (bytes) // behaviour of the write function with 0 bytes is undefined
	{
		size_t written = write(pipe_in[1], buffer, bytes);
		if (static_cast<int>(written) == -1)
			stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", true);
		else if (written != bytes)
			stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", true);
	}
	if (content_length == 0 && is_error == false)
	{
		close(pipe_in[1]);
		wait(NULL);
	}
}

void Request::unchunk_body(std::istringstream& data)
{
	char			buffer[4001];
	std::string		line;
	size_t			written = 0;
	size_t			avail = 0;
	std::cout << "unchunk_body " << parsing_position << std::endl;
	if (parsing_position == done_with_header)
		parsing_position = first_chunk_size;
	do
	{
		if (remove_n)
		{
			remove_n = false;
			continue;
			// std::cout << "remove_n" << remove_n << std::endl;
			// data.ignore(remove_n);
		}
		if (missing_chuncked_data == 0)
		{
			if (ft::is_whitespace(line) == EXIT_SUCCESS && part_of_hex_of_chunked.empty())
			{
				std::cout << "continue " << line;
				continue;
			}
			std::cout << "missing_chuncked_data == 0" << std::endl;
			line = part_of_hex_of_chunked + line;
			part_of_hex_of_chunked.clear();
			if (line.find('\r') != std::string::npos)
			{
				std::cout << "find r" << std::endl;
				line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); //remove the newlines
				missing_chuncked_data = ft::Str_to_Hex_to_Int(line);
				std::cout << "chunk size=" << missing_chuncked_data << "line" << line << "." << std::endl;
				if (missing_chuncked_data == 0)
				{
					std::getline(data, line);
					line.clear();
					if (parsing_position == first_chunk_size)
						status_line = "HTTP/1.1 405 Method Not Allowed";
					std::cout << "done with File" << std::endl << std::flush;
					int ret;
					close(pipe_in[1]);
					waitpid(pid_child, &ret, 0);
					rewind(out_file);
					std::cout << "child return = " << ret << std::endl;
					parsing_position = send_first;
					break;
				}
				else
				{
					parsing_position = body;
				}
			}
			else
			{
				part_of_hex_of_chunked = line;
				std::cout << "part_of_hex_of_chunked is now " << part_of_hex_of_chunked << std::endl;
			}
			if(avail == line.length() + 1)
			{
				remove_n = true;
				std::cout << "remove_n=" << remove_n << std::endl;
			}
			// std::cout << "missing_dat: " << "." << missing_chuncked_data << " vs " << data.rdbuf()->in_avail() << "." << std::endl;
		}
		int read_bytes = 0;
		if(data.rdbuf()->in_avail())
		{
			written = 0;
			if (missing_chuncked_data > static_cast<size_t>(data.rdbuf()->in_avail()))
			{
				// std::cout << "write whole array" << std::endl;
				read_bytes = data.rdbuf()->in_avail();
				read_bytes = data.readsome(buffer, read_bytes);
			}
			else
			{
				std::cout << "write missing_chuncked_data " << missing_chuncked_data << std::endl;
				read_bytes = data.readsome(buffer, missing_chuncked_data);
			}
			if(std::string(buffer, read_bytes).find("\n") != std::string::npos)
			{
				std::cout << "why" << std::string(buffer, read_bytes) << std::string(buffer, read_bytes).find("\n") <<" part pf hex nb"<< part_of_hex_of_chunked << "missing_chuncked_data" << missing_chuncked_data << " left in buffer" << data.rdbuf()->in_avail() << std::endl;
				// std::string tmp = std::string(buffer, read_bytes).erase(std::string(buffer, read_bytes).find("\n"), 1);
				// std::strcpy(buffer, &tmp[0]);
				// read_bytes -= 1;
			}
			if(read_bytes)
				written = write(pipe_in[1], buffer, read_bytes);
			if (static_cast<int>(written) == -1)
				stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", true);
			else if (static_cast<int>(written) != read_bytes)
				stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", true);
			missing_chuncked_data -= written;
		}
		// std::cout <<"written = " << written << "read_bytes = " << read_bytes << std::endl;
		// std::cout << "missing_dat: " << "." << missing_chuncked_data << " vs " << data.rdbuf()->in_avail() << " written:" << written << "." << std::endl;
			std::cout << "missing_chuncked_data=" << missing_chuncked_data << std::endl;

	} while (data && (avail = data.rdbuf()->in_avail()) && std::getline(data, line));
	std::cout << "after chunked" << std::endl;
}

void		Request::stop_reading(std::string status, bool close_fd)
{
	std::cout << "stop_reading" << std::endl;
	set_status_line(status);
	set_error_status(true);
	if (close_fd)
	{
		close(pipe_in[1]);
		wait(NULL);
		// waitpid(pid_child, NULL, 0); //or waitpid?
	}
}

bool Request::is_content_length_valid()
{
	if (content_length <= max_body)
	{
		return (true);
	}
	return (false);
}