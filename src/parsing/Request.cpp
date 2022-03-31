#include "Request.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>
#include <strings.h>
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
	while ((pos = s.find(" ")) != std::string::npos)
	{
		token = s.substr(0, pos);
		if ((token.length()) > 0 && j == 0)
		{
			this->method = token;
			j++;
		}
		else if (token.length() > 0 && j++ == 1)
			this->path = token;
		s.erase(0, pos + 1);
	}
	this->protocol = s;
	return (EXIT_SUCCESS);
}

int Request::set_headers(std::string line)
{
	std::cout << "set_headers" << "line=" << line << ".= " << std::endl;
	size_t pos;
	// std::cout << line << std::endl;
	if(line.find("\r") == std::string::npos)
	{
		// std::cout << "doesent fond /r in line" << std::endl;
		raw_header_line += line;
		return (EXIT_FAILURE);
	}
	if (raw_header_line.empty() == false)
	{
		line = raw_header_line + line;
		raw_header_line.clear();
	}
	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); //remove the newlines
	// if(line.find("\r") == std::string::npos)
	// {
	// 	std::cout << "doesent fond /r in line2" << std::endl;
	// }
	if (line.empty() || ft::is_whitespace(line) == EXIT_SUCCESS)
	{
		// std::cout << "emty line in end of header" << std::endl;
		parsing_position = done_with_header;
		return (EXIT_FAILURE);
	}
	pos = line.find(":");
	if (!(line[0] == '/' || line[0] == '#'))
		headers[line.substr(0, pos)] = line.substr(pos + 2, line.length());
	return (EXIT_SUCCESS);
}

void Request::for_testing_print_request_struct()
{
	std::cout << "\n---START LINE \n";

	std::cout << this->method << " " << this->path << " " << this->protocol << "\n";
	std::map<std::string, std::string>::iterator it;
	std::cout << "\n---HEADERS \n";
	for (it = this->headers.begin(); it != this->headers.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << '\n';
	}
}

Request::Request()
{
	parsing_position = first_line;
	missing_chuncked_data = 0;
	set_error_status(false);
	status_line = "HTTP/1.1 200 OK";
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

	while ((parsing_position == first_line) && data && std::getline(data, line))
	{
		if(line.empty())
			continue;
		if (set_start_line(line) == EXIT_SUCCESS)
			break;
	}

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
		std::cout << "fd = " << fd << std::endl;
		for_testing_print_request_struct();
		if (method == "POST" || method == "PUT") // check if it is a post request
		{
			if (content_length != 0)
			{
				if (pipe(pipe_in) == -1)
				{
					stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", false);
					return ;
				}
				pid_child = fork(); // 2x pid_child variables
				std::cout << "fork" <<std::endl;
				if (pid_child == -1)
				{
					stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", false);
					return ;
				}
				else if (pid_child == 0)
				{
					std::vector<char *> env;
					std::vector<std::string> env_strings;
					std::map<std::string, std::string>::iterator tmp;
					if ((tmp = headers.find("Content-Length")) != headers.end())
					{
						env_strings.push_back("CONTENT_LENGTH=" + tmp->second);
					}
					env_strings.resize(20);
					env_strings.push_back("QUERY_STRING=");
					env_strings.push_back("REQUEST_URI=" + get_path());
					env_strings.push_back("REDIRECT_STATUS=200");
					env_strings.push_back("SCRIPT_NAME=" + get_path());
					if(method == "POST")
						env_strings.push_back("SCRIPT_FILENAME=." + get_path());
					else
						env_strings.push_back("SCRIPT_FILENAME=./cgi-bin/cgi.php");
					// std::cout << env_strings.back() << std::endl;
					// env_strings.push_back("DOCUMENT_ROOT=");
					env_strings.push_back("REQUEST_METHOD=" + method);
					env_strings.push_back("SERVER_PROTOCOL=" + protocol);
					env_strings.push_back("SERVER_SOFTWARE=webserv");
					env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");
					env_strings.push_back("REQUEST_SCHEME=http");
					if ((tmp = headers.find("Host")) != headers.end())
					{
						env_strings.push_back("SERVER_PORT=" + tmp->second);
						env_strings.back().erase(std::strlen("SERVER_PORT=") - 1, env_strings.back().find(":"));
					}
					env_strings.push_back("SERVER_ADDR=");
					if ((tmp = headers.find("Host")) != headers.end())
					{
						env_strings.push_back("SERVER_PORT=" + tmp->second);
						env_strings.back().erase(env_strings.back().find(":") - 1, env_strings.back().length() - 1);
					}

					if ((tmp = headers.find("Content-Type")) != headers.end())
					{
						env_strings.push_back("CONTENT_TYPE=" + tmp->second);
					}
					// env_strings.push_back("=");
					// env_strings.push_back("REDIRECT_STATUS=200");
					// env_strings.push_back("REDIRECT_STATUS=200");
					// env_strings.push_back("REDIRECT_STATUS=200");
					// env_strings.push_back("REDIRECT_STATUS=200");

					for(size_t i = 0; i < env_strings.size(); i++)
					{
						env.push_back(&(env_strings[i][0]));
					}
					env.push_back(NULL);
					close(pipe_in[1]);
					dup2(pipe_in[0], STDIN_FILENO); //stop reading
					dup2(STDERR_FILENO, STDOUT_FILENO); //stop reading
					char * const * nll = NULL;
					if (execve("./cgi-bin/php-cgi", nll, &env[0]))
					{
						perror("execvp"); //stop reading
						exit(EXIT_FAILURE);
					}
					exit(EXIT_FAILURE);
				}
				close(pipe_in[0]);
			}
		}
		else
		{
				parsing_position = done;
		}
	}
	if (parsing_position >= done_with_header)
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


// ----------------- GETTERS ------------------ //

std::string Request::get(std::string key_word)
{
	std::map<std::string, std::string>::iterator itr;
	if (key_word == "method")
		return (method);
	else if (key_word == "path")
		return (path);
	else if (key_word == "protocol")
		return (protocol);
	else
	{
		itr = headers.find(key_word);
		if (itr == headers.end())
			return (key_word);
		else
			return (std::string());
	}
}

std::map<std::string, std::string> Request::get_header() const
{
	return (headers);
}
std::string Request::get_method() const
{
	return (method);
}

bool Request::get_error_status() const
{
	return (is_error);
}

std::string Request::get_path() const
{
	return (path);
}
std::string Request::get_protocol() const
{
	return (protocol);
}

std::string Request::get_port()
{
	std::string port;
    if (headers.find("Host") != headers.end())
    {
        std::string host = headers.find("Host")->second;
        port = host.substr(host.find_last_of(':') + 1, host.length());
    }
    return (port);
}

int Request::get_content_length()
{
	std::map<std::string, std::string>::iterator tmp = headers.find("Content-Length");
	if (tmp != headers.end())
		return (ft::stoi(tmp->second));
	return (-1);
}

std::string		Request::get_status_line() const
{
	return(status_line);
}

int		Request::get_parsing_position() const
{
	return(parsing_position);
}


// --------------------- CHECKERS ---------------------

bool Request::is_chunked(void)
{
    if (headers.find("Content-Length") == headers.end())
    {
        // std::cout << "Chunked branch\n";
        // std::cout << "down\n";
        // for_testing_print_request_struct();
        // std::cout << "\nup\n";
        // std::cout << "~~For debugging purposes.";
        //exit(1);
        return (true);
    }
    return (false);
}

// --------------- OVERLOADS ---------------- //

void Request::set_regular_body(std::istringstream& data)
{
	char buffer[4001];
	// size_t bytes = recv(fd, buffer, 4000, 0);
	// content_length -= static_cast<int>(bytes);
	std::cout << ",Len: " << content_length;
	// if (static_cast<int>(bytes) == -1)
	// 	stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", true); //What happens with the partially uploaded file?
	// else if (static_cast<int>(bytes) == 0 && content_length != 0)
	// 	stop_reading("HTTP/1.1 400 BAD REQUEST", true);
	// std::istringstream data(std::string(buffer, bytes), std::ios::binary);
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

// void Request::set_chunked_body(int fd)
// {
// 	char buffer[4001];
// 	bzero(buffer, 4001);
// 	size_t bytes = recv(fd, buffer, 4000, 0);
// 	std::istringstream data(std::string(buffer, bytes), std::ios::binary);
// 	std::cout << "bytes" << bytes <<  std::endl;
// 	if (static_cast<int>(bytes) == -1)
// 		stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", true);
// 	else if ((int)bytes == 0) // How to check if not enough body was sent in the chunked request? Stop reading
// 	{
// 		stop_reading("HTTP/1.1 400 BAD REQUEST", true);
// 	}
// 	unchunk_body(data);
// }

void Request::unchunk_body(std::istringstream& data)
{
	char buffer[4001];
	std::string line;
	size_t written = 0;
	std::cout << "unchunk_body " << parsing_position << std::endl;
	if (parsing_position == done_with_header)
		parsing_position = first_chunk_size;
	do
	{
		if (missing_chuncked_data == 0)
		{
			// std::cout << "chunked beginn" << "." << line << "." << std::endl;
			// std::cout << data << std::endl;
			if (ft::is_whitespace(line) == EXIT_SUCCESS && part_of_hex_of_chunked.empty())
				continue;
			line = part_of_hex_of_chunked + line;
			part_of_hex_of_chunked.clear();
			if (line.find('\r') != std::string::npos)
			{
				line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); //remove the newlines
				missing_chuncked_data = ft::Str_to_Hex_to_Int(line);
				std::cout << "chunk size=" << missing_chuncked_data << std::endl;
				if (missing_chuncked_data == 0)
				{
					std::getline(data, line);
					line.clear();
					if (parsing_position == first_chunk_size)
						status_line = "HTTP/1.1 405 Method Not Allowed";
					// std::cout << "close pipe" << std::endl;
					std::cout << "done with File" << std::endl << std::flush;

					close(pipe_in[1]);
					waitpid(pid_child, NULL, 0);
					parsing_position = done;
					break;
				}
				else
				{
					parsing_position = body;
				}
			}
			else
				part_of_hex_of_chunked = line;
			// std::cout << missing_chuncked_data << std::endl;
			// std::cout << "missing_dat: " << "." << missing_chuncked_data << " vs " << data.rdbuf()->in_avail() << "." << std::endl;
		}
		int read_bytes;
		if (missing_chuncked_data > data.str().length())
		{
			// std::cout << "write whole array" << std::endl;
			read_bytes = data.rdbuf()->in_avail();
				std::cout << "read_bytes" << read_bytes << std::endl;
			read_bytes = data.readsome(buffer, read_bytes);
				std::cout << "read_bytes" << read_bytes << std::endl;
			if (read_bytes)
			{
				written = write(pipe_in[1], buffer, read_bytes);
				std::cout << "read_bytes" << read_bytes << std::endl;
			}
		}
		else
		{
			// std::cout << "write missing_chuncked_data" << std::endl;
			read_bytes = data.readsome(buffer, missing_chuncked_data);
			if(read_bytes)
				written = write(pipe_in[1], buffer, missing_chuncked_data);
			// write(STDOUT_FILENO, buffer, missing_chuncked_data);
		}
		std::cout <<"written = " << written << "read_bytes = " << read_bytes << std::endl;
		if (static_cast<int>(written) == -1)
			stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", true);
		else if (static_cast<int>(written) != read_bytes)
			stop_reading("HTTP/1.1 500 INTERNAL SERVER ERROR", true);
		missing_chuncked_data -= written;
		std::cout << "missing_dat: " << "." << missing_chuncked_data << " vs " << data.rdbuf()->in_avail() << " written:" << written << "." << std::endl;
	} while (data && std::getline(data, line));
}

void		Request::stop_reading(std::string status, bool close_fd)
{
	set_status_line(status);
	set_error_status(true);
	if (close_fd)
	{
		close(pipe_in[1]);
		wait(NULL);
		// waitpid(pid_child, NULL, 0); //or waitpid?
	}
}
