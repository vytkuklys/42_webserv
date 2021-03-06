#include "Request.hpp"
#include "cstring"

#include <signal.h>

Request::Request(SERVER::WebServer& tmp_webserver):
 webserver(tmp_webserver)
,content_length(-1)
,missing_chuncked_data(0)
,pid_child(-1)
,parsing_position(read_first_line)
,remove_n(false)
,chunked_size(0)
,status_code(200)
,http_response()
{
	pipe_in[0] = -1;
	pipe_in[1] = -1;
	time_of_change = ft::time();
}

Request::~Request()
{
	return;
}

bool Request::set_start_line(std::string line)
{
	size_t pos = 0;
	std::string token;
	int j = 0;
	if(line.find("\r") == std::string::npos)
	{
		raw_header_line += line;
		return (EXIT_FAILURE);
	}
	parsing_position = read_header;
	if (raw_header_line.empty() == false)
	{
		line = raw_header_line + line;
		raw_header_line.clear();
	}
	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
	if (line[0] == '/' || line[0] == '#')
		return (EXIT_FAILURE);
	while ((pos = line.find(" ")) != std::string::npos)
	{
		token = line.substr(0, pos);
		if ((token.length()) > 0 && j == 0)
		{
			set_method(token);
			j++;
		}
		else if (token.length() > 0 && j++ == 1)
			set_path(token);
		line.erase(0, pos + 1);
	}
	set_protocol(line);
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
	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
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

void	Request::set_up_child()
{
	//set up argv;
	std::vector<char *> argv;
	std::vector<std::string> argv_strings;
	argv_strings.resize(2);
	argv_strings.push_back(config->getScript());
	argv_strings.push_back(get_path());
	for(size_t i = 0; i < argv_strings.size(); i++)
		argv.push_back(&(argv_strings[i][0]));
	argv.push_back(NULL);

	//set up env
	std::vector<char *> env;
	std::vector<std::string> env_strings;
	env_strings.resize(40);
	std::string tmp;
	if ((tmp = get_value("Content-Length")) != "not found")
		env_strings.push_back("CONTENT_LENGTH=" + tmp);
	env_strings.push_back("PATH_INFO=" + get_path());
	env_strings.push_back("QUERY_STRING=");
	env_strings.push_back("REDIRECT_STATUS=200");
	env_strings.push_back("DOCUMENT_ROOT=" + config->getRoot());
	env_strings.push_back("REQUEST_METHOD=" + get_method());
	env_strings.push_back("SERVER_PROTOCOL=" + get_protocol());
	env_strings.push_back("SERVER_SOFTWARE=webserv");
	env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env_strings.push_back("REQUEST_SCHEME=http");
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
	}
	if(config->getScript() == "./php-cgi")
	{
		env_strings.push_back("SCRIPT_FILENAME=./cgi.php");
		env_strings.push_back("SCRIPT_NAME=./cgi.php");
	}
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
		env.push_back(&(env_strings[i][0]));
	env.push_back(NULL);
	webserver.close_all_pipes();
	if (dup2(pipe_in[0], STDIN_FILENO) == -1)
	{
		std::cout << "close2" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (close(pipe_in[0]) != 0)
	{
		std::cout << "close3" << std::endl;
		exit(EXIT_FAILURE);
	}
	pipe_in[0] = -1;
	if (dup2(fileno(out_file), STDOUT_FILENO) == -1)
	{
		std::cout << "close4" << std::endl;
		exit(EXIT_FAILURE);
	}
	close(fileno(out_file));
	chdir("./cgi-bin");
	if (execve(config->getScript().c_str(), &argv[0], &env[0]))
	{
		std::cout << "script=" << config->getScript().c_str() << std::endl;
		perror("execve");
		exit(EXIT_FAILURE);
	}
	perror("execve");
	exit(EXIT_FAILURE);
}


void			Request::set_up_cgi_proces()
{
	if (content_length == -1)
	{
		content_length = get_content_length();

		if (is_payload_too_large() == true)
		{
			std::cerr << "is_payload_too_large" << std::endl;
			stop_reading(413);
			return ;
		}
	}

	if (get_method() == "POST" || get_method() == "PUT")
	{
		if (pipe(pipe_in) == -1)
		{
			std::cerr << "pipe creating goes wrong" << std::endl;
			stop_reading(500);
			return ;
		}
		out_file = tmpfile();
		pid_child = fork();
		if (pid_child == -1)
		{
			std::cerr << "fork goes wrong" << std::endl;
			stop_reading(500);
			close(pipe_in[0]);
			close(pipe_in[1]);
			pipe_in[0] = -1;
			pipe_in[1] = -1;
			return ;
		}
		else if (pid_child == 0)
			set_up_child();
		else
		{
			if(close(pipe_in[0]) != 0)
			{
				std::cout << "close5" << std::endl;
				exit(EXIT_FAILURE);
			}
			pipe_in[0] = -1;
		}

	}
	else
		parsing_position = send_first;
}

void	Request::fill_header(int fd, Config& conf)
{
	char buffer[ARRAY_SIZE];
	std::string line;
	size_t bytes = recv(fd, buffer, ARRAY_SIZE, 0);

	if (static_cast<int>(bytes) == -1)
	{
		std::cerr << "recv" << std::endl;
		stop_reading(500);
		return ;
	}
	else if (static_cast<int>(bytes) == 0)
	{
		stop_reading(400);
		return ;
	}

	std::istringstream data(std::string(buffer, bytes), std::ios::binary);

	while ((parsing_position == read_first_line) && data && std::getline(data, line))
	{
		if(line.empty())
			continue;
		if (set_start_line(line) == EXIT_SUCCESS)
			break;
	}

	while ((parsing_position == read_header) && data && std::getline(data, line))
	{
		if(line.empty())
			continue;
		if (set_headers(line) == EXIT_FAILURE)
			break;
	}

	if (parsing_position == done_with_header)
	{
		config = conf.get_location(get_hostname(), get_path());
		if(config == NULL)
		{
			parsing_position = send_first;
			status_code = 400;
		}
		else
			set_up_cgi_proces();
	}
	if (status_code >= 400)
		return ;
	if (parsing_position >= done_with_header && parsing_position < send_first)
	{
		if (is_chunked())
		{
			unchunk_body(data);
		}
		else
			set_regular_body(data);
	}
}
// ----------------- SETTER ------------------ //

void	Request::set_status_code(int code)
{
	status_code = code;
}

void	Request::set_parsing_position(mile_stones new_pos)
{
	parsing_position = new_pos;
}

void Request::set_regular_body(std::istringstream& data)
{
	parsing_position = body;
	char buffer[ARRAY_SIZE];
	size_t bytes = data.rdbuf()->in_avail();
	bytes = data.readsome(buffer, bytes);
	if (bytes)
	{
		size_t written = write(pipe_in[1], buffer, bytes);
		if (static_cast<int>(written) == -1)
		{
			std::cerr << "write in pipe goes wrong" << std::endl;
			stop_reading(500);
			return ;
		}
		else if (written != bytes)
		{
			return;
		}
		content_length -= written;
		std::cout << "content_length" << content_length << std::endl;

		if (content_length <= 0 && status_code < 400)
		{
			if (close(pipe_in[1]) != 0)
			{
				std::cout << "close1" << std::endl;
				exit(EXIT_FAILURE);
			}
			pipe_in[1] = -1;
			wait_for_child();
			fclose(out_file);
			parsing_position = send_first;
		}
	}
}

// ----------------- GETTERS ------------------ //

int Request::get_content_length()
{
	std::string tmp = get_value("Content-Length");

	if(tmp == "not found")
		return (0);
	return (ft::stoi(tmp));
}

int		Request::get_status_code() const
{
	return(status_code);
}

int		Request::get_parsing_position() const
{
	return(parsing_position);
}

std::string		Request::get_cgi_return()
{
	char tmp[ARRAY_SIZE];
	int bytes;
	if (!http_response.empty())
	{
		return(http_response);
	}
	else if ((bytes = fread(tmp, 1, ARRAY_SIZE, out_file)) <= 0)
	{
		fclose(out_file);
	}
	if (parsing_position == erase_cgi_header)
	{
		std::string tmp1(tmp, bytes);
		parsing_position = send_body;
		return(tmp1.erase(0, tmp1.find("\r\n\r\n") + 4));
	}
	return(std::string(tmp, bytes));
}

void Request::set_http_response(std::string tmp_http_response)
{
	http_response = tmp_http_response;
}

unsigned long	Request::get_time_of_change()
{
	return(time_of_change);
}

LocationData*	Request::get_config()
{
	return(config);
}



// --------------------- CHECKERS ---------------------

bool Request::is_chunked(void)
{

    if ((get_method() == "POST" || get_method() == "PUT") && get_content_length() == 0)
    {
        return (true);
    }
    return (false);
}

bool	Request::is_chunked_payload_too_large(void)
{
	int max = 1;
	if (config != NULL)
	{
		max = config->getMaxBody();
	}
	if (chunked_size > max)
	{
		return (true);
	}
	return (false);
}

bool Request::is_payload_too_large()
{
	int max = 1;
	if (config != NULL)
	{
		max = config->getMaxBody();
	}
	if (content_length <= max)
	{
		return (false);
	}

	return (true);
}

void Request::wait_for_child()
{
	int	ret;
	int ret_wa;
	if (pid_child != -1)
	{
		ret_wa = waitpid(pid_child, &ret, 0);
		if (ret_wa == -1)
		{
			stop_reading(500);
		}
		if (ret_wa == 0)
		{
			std::cout << "waitpid fine" << std::endl;
		}
	}
}

void	Request::close_pipe_in()
{
	if(pipe_in[1] != -1)
	{
		if (close(pipe_in[1]) == -1)
			std::cout << "error close";
		pipe_in[1] = -1;
	}
}

bool Request::proces_chunked_size(std::string& line)
{
	if (line.find('\r') != std::string::npos)
	{
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		missing_chuncked_data = ft::Str_to_Hex_to_Int(line);
		if (missing_chuncked_data == 0)
		{
			if (parsing_position == read_first_chunk_size)
				status_code = 204;
			else if (is_payload_too_large() == true)
			{
				std::cerr << "is_payload_too_large1" << std::endl;
				status_code = 413;
			}
			int ret;
			if (close(pipe_in[1]) != 0)
				std::cout << "error close" << std::endl;
			pipe_in[1] = -1;
			waitpid(pid_child, &ret, 0);
			pid_child = -1;
			rewind(out_file);
			parsing_position = send_first;
			return(false);
		}
		else
		{
			parsing_position = body;
		}
	}
	else
	{
		part_of_hex_of_chunked = line;
	}
	return(true);
}

bool Request::proces_chunked_body(std::istringstream& data)
{
	size_t		written = 0;
	size_t		read_bytes = 0;
	char	buffer[ARRAY_SIZE];
	if (missing_chuncked_data > static_cast<size_t>(data.rdbuf()->in_avail()))
	{
		read_bytes = data.rdbuf()->in_avail();
		read_bytes = data.readsome(buffer, read_bytes);
	}
	else
		read_bytes = data.readsome(buffer, missing_chuncked_data);
	if(read_bytes)
		written = write(pipe_in[1], buffer, read_bytes);
	if (static_cast<int>(written) == -1)
	{
		std::cerr << "writte in pipe goes wrong" << std::endl;
		stop_reading(500);
		return(false) ;
	}
	else if (written != read_bytes)
	{
		stop_reading(500);
		return(false) ;
	}
	missing_chuncked_data -= written;
	chunked_size += written;
	if (is_chunked_payload_too_large() == true)
	{
		std::cerr << "is_chunked_payload_too_large" << std::endl;
		stop_reading(413);
		return(false) ;
	}
	return(true);
}

void Request::unchunk_body(std::istringstream& data)
{

	std::string		line;
	size_t			avail = 0;

	if (parsing_position == done_with_header)
		parsing_position = read_first_chunk_size;
	do
	{
		if (remove_n)
		{
			remove_n = false;
			continue;
		}
		if (missing_chuncked_data == 0)
		{
			if (ft::is_whitespace(line) == EXIT_SUCCESS && part_of_hex_of_chunked.empty())
			{
				continue;
			}
			if(avail == line.length())
			{
				remove_n = true;
			}
			line = part_of_hex_of_chunked + line;
			part_of_hex_of_chunked.clear();
			if (proces_chunked_size(line) == false)
				return;
		}

		if(data.rdbuf()->in_avail())
		{
			if (proces_chunked_body(data) == false)
				return;
		}
	} while (data && (avail = data.rdbuf()->in_avail()) && std::getline(data, line));
}

void		Request::stop_reading(int code)
{
	std::cerr << "stop_reading, request: " << code << std::endl;
	status_code = code;
	if (pipe_in[1] != -1)
	{
		if (close(pipe_in[1]) != 0)
		{
			std::cout << "close6" << std::endl;
			exit(EXIT_FAILURE);
		}
		pipe_in[1] = -1;
	}
	if (pid_child != -1)
	{
		waitpid(pid_child, NULL, 0);
		pid_child = -1;
	}
}
