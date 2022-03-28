#include "Request.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>
#include <strings.h>
bool Parsing::set_start_line(std::string s)
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

int Parsing::set_headers(std::string line)
{
	size_t pos;
	// std::cout << line << std::endl;
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
		parsing_position = first_body;
		return (EXIT_FAILURE);
	}
	pos = line.find(":");
	if (!(line[0] == '/' || line[0] == '#'))
		headers[line.substr(0, pos)] = line.substr(pos + 2, line.length());
	return (EXIT_SUCCESS);
}

void Parsing::for_testing_print_request_struct()
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

Parsing::Parsing(int fd)
{
	parsing_position = first_line;
	status_line = "HTTP/1.1 200 OK";
	fill_header(fd);
}

Parsing::~Parsing()
{
	return;
}

void	Parsing::fill_header(int fd)
{
	char buffer[4001];
	std::string line;
	size_t bytes = recv(fd, buffer, 4000, 0);
	// std::cout << "bysts=" << bytes << "\nbuffer\n" << buffer << std::endl;
	write(1, buffer, bytes);
	std::istringstream data(std::string(buffer, bytes), std::ios::binary);
	// std::cout << "missing_dat: " << "." << data.str().length() << "." << std::endl;

	// bytes += 1; //not nessesary ?

	while ((parsing_position == first_line) && data && std::getline(data, line) && set_start_line(line))
		;
	// std::cout << line << std::endl;
	while ((parsing_position == header) && data && std::getline(data, line) && set_headers(line) == EXIT_SUCCESS)
	{
			// std::cout << line << std::endl;
		// std::cout << line << data <<std::endl;
	}
	// std::cout << line << std::endl;


	// std::cout << "\n"
	// 		  << path << "\n";
	std::map<std::string, std::string>::iterator length_location = headers.find("Content-Length");
	int length = 1;
	if (length_location != headers.end())
		length = ft::stoi(length_location->second);
	if (parsing_position == first_body && method == "POST" && length > 0) // check if it is a post request
	{
		// std::cout << method << std::endl;
		if (pipe(pipe_in) == -1)
			std::cout << "pipe error" << std::endl;
		int pid_child = fork();
		if (pid_child == -1)
			std::cout << "eror fork" << std::endl;
		if (pid_child == 0)
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
			env_strings.push_back("SCRIPT_FILENAME=." + get_path());
			std::cout << env_strings.back() << std::endl;
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
			dup2(pipe_in[0], STDIN_FILENO);
			dup2(STDERR_FILENO, STDOUT_FILENO);
			char * const * nll = NULL;
			if (execve("/Users/shackbei/goinfre/.brew/bin/php-cgi", nll, &env[0]))
			{
				perror("execvp");
				exit(EXIT_FAILURE);
			}
		}
		close(pipe_in[0]);
		if (!is_chunked())
		{
			int test = 0;
			while (data && std::getline(data, line))
			{
				write(pipe_in[1], line.c_str(), line.length());
				// std::cout << "Line: " << line << std::endl;
				test++;
			}
			// std::cout << "loops after reading header in POST: " << test << "\n";
		}
		else
		{
			missing_chuncked_data = 0;
			std::cout << "new chuked" << std::endl;
			unchunk_body(data);
		}
	}
}
// ----------------- SETTER ------------------ //

void	Parsing::set_status_line(std::string new_status)
{
	status_line = new_status;
}


// ----------------- GETTERS ------------------ //

std::string Parsing::get(std::string key_word)
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

std::map<std::string, std::string> Parsing::get_header() const
{
	return (headers);
}
std::string Parsing::get_method() const
{
	return (method);
}
std::string Parsing::get_path() const
{
	return (path);
}
std::string Parsing::get_protocol() const
{
	return (protocol);
}
std::string Parsing::get_body() const
{
	return (body);
}


std::string Parsing::get_port()
{
	std::string port;
    if (headers.find("Host") != headers.end())
    {
        std::string host = headers.find("Host")->second;
        port = host.substr(host.find_last_of(':') + 1, host.length());
    }
    return (port);
}

int Parsing::get_content_length()
{
	std::map<std::string, std::string>::iterator tmp = headers.find("Content-Length");
	if (tmp != headers.end())
		return (ft::stoi(tmp->second));
	return (-1);
}

std::string		Parsing::get_status_line() const
{
	return(status_line);
}

int		Parsing::get_parsing_position() const
{
	return(parsing_position);
}


// --------------------- CHECKERS ---------------------

bool Parsing::is_chunked(void)
{
    if (headers.find("Content-Length") == headers.end())
    {
        // std::cout << "Chunked branch\n";
        // std::cout << "down\n";
        // for_testing_print_request_struct();
        // std::cout << "\nup\n";
        // std::cout << "~~For debugging purposes.";
        // exit(1);
        return (true);
    }
    return (false);
}

// --------------- OVERLOADS ---------------- //

void Parsing::set_regular_body(int fd)
{
	char buffer[4001];
	size_t bytes = recv(fd, buffer, 4000, 0);
	std::istringstream data(std::string(buffer, bytes), std::ios::binary);
	if ((int)bytes == -1)
	{
		// do something
	}
	else if ((int)bytes == 0)
	{
		// do somethings else
	}
	size_t written = write(pipe_in[1], data.str().c_str(), bytes);
	if ((int)written == -1)
	{
		// do something
	}
	else if (written == 0)
	{
		// do something else
	}
	if ((int)bytes < 4000)
	{
		close(pipe_in[1]);
		wait(NULL);
	}
}

void Parsing::set_chunked_body(int fd)
{
	char buffer[4001];
	bzero(buffer, 4001);
	size_t bytes = recv(fd, buffer, 4000, 0);
	std::istringstream data(std::string(buffer, bytes), std::ios::binary);
	std::cout << "." << buffer << "." <<  std::endl;
	if ((int)bytes == -1)
	{
		// do something
	}
	else if ((int)bytes == 0)
	{
		// do somethings else
	}
	unchunk_body(data);
}

void Parsing::unchunk_body(std::istringstream& data)
{
	char buffer[4001];
	std::string line;
	size_t written;
	std::cout << "unchunk_body" << std::endl;
	do
	{
		if (missing_chuncked_data == 0)
		{
			// std::cout << "chunked beginn" << "." << line << "." << std::endl;
			if (ft::is_whitespace(line) == EXIT_SUCCESS && part_of_hex_of_chunked.empty())
				continue;
			line = part_of_hex_of_chunked + line;
			part_of_hex_of_chunked.clear();
			if (line.find('\r') != std::string::npos)
			{
				line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); //remove the newlines
				missing_chuncked_data = ft::Str_to_Hex_to_Int(line);
				if (missing_chuncked_data == 0)
				{
					std::getline(data, line);
					if (parsing_position == first_body)
						status_line = "HTTP/1.1 405 Method Not Allowed";
					// std::cout << "close pipe" << std::endl;
					close(pipe_in[1]);
					waitpid(pid_child, NULL, 0);
					break;
				}
			}
			else
				part_of_hex_of_chunked = line;
			// std::cout << missing_chuncked_data << std::endl;
			// std::cout << "missing_dat: " << "." << missing_chuncked_data << " vs " << data.rdbuf()->in_avail() << "." << std::endl;
		}
		if (missing_chuncked_data > data.str().length())
		{
			// std::cout << "write whole array" << std::endl;
			int tmp_size = data.rdbuf()->in_avail();
			tmp_size = data.readsome(buffer, tmp_size);
			written = write(pipe_in[1], buffer, tmp_size);
		}
		else
		{
			// std::cout << "write missing_chuncked_data" << std::endl;
			data.readsome(buffer, missing_chuncked_data);
			written = write(pipe_in[1], buffer, missing_chuncked_data);
			// write(STDOUT_FILENO, buffer, missing_chuncked_data);
		}
		if (static_cast<int>(written) == -1)
		{
			std::cout << "error write -1" << std::endl;
		}
		else if (written == 0)
		{
			std::cout << "error write 0" << std::endl;
		}
		missing_chuncked_data -= written;
		// std::cout << "missing_dat: " << "." << missing_chuncked_data << " vs " << data.rdbuf()->in_avail() << " written:" << written << "." << std::endl;
	} while (data && std::getline(data, line));
}

