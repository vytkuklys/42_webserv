#include "Response.hpp"


Response::Response(Request& req, Config& data) : request(&req)
{
	config = &data;
    default_error = "./documents/html_errors";
	default_err = "/404.html";
	if (request->get_error_status() == true)
	{
		set_error_path();
	}
	else if (::is_redirection(request->get_path()))
	{
		set_redirection_status(true);
	}
	else
	{
		std::cout << "Responder constructor" << std::endl;
    	set_path(request->get_path());
		if (request->get_method() == "DELETE")
		{
			handle_delete_request();
		}
	}
    file_ext = path.substr(path.find_last_of('.') + 1, path.length());
	if (request->get_method() != "HEAD")
		set_body();
    set_headers();
}

// ----------------- MEMBERS ------------------- //
void	Response::stop_writing(void)
{
	std::cout << "stop_writing, response" << std::endl;
	request->set_status_code(500);
	set_error_path();
	file_ext = "html";
	set_body();
}

// ----------------- SETTERS ------------------- //

void Response::set_error_path(void)
{
	int status = request->get_status_code();
	path = config->getErrorPage(request->get_hostname());
    if (path.empty())
	{
        path = default_error;
	}
	if (status == 500)
	{
		path.append("/500.html");
	}
	else if (status == 400)
	{
		path.append("/400.html");
	}
	else if (status == 413)
	{
		path.append("/413.html");
	}
}

void Response::set_path(std::string const filename)
{
    path = filename;
    std::string host = request->get_hostname();
    bool is_listing_on = config->getDirectoryListing(host);
    LocationData * loc = config->get_location(host, filename);
	bool is_host = config->getHostStatus();
	default_err = config->getDefaultErr(host);
    if (is_host == false)
	{
		request->set_status_code(400);
		set_error_page("/400.html");
	}
	else if (loc != nullptr)
	{
		if((loc->getLocation() == "/" && loc->getLocation().length() == 1))
			path.insert(0, loc->getRoot());
		else
			ft::replace(path, loc->getLocation(), loc->getRoot());

		if(exists_dir(path))
		{
			if(path[path.length() - 1] != '/')
				path += "/";
			path += loc->getIndex();
		}
        bool has_access = is_authorized(loc->getMethod(), request->get_method(), (!is_listing_on && filename == "/index.php"));
		if (has_access == false)
		{
			request->set_status_code(405);
		}
		if(request->get_status_code() < 400)
		{
			if (request->get_method() == "PUT")
				request->set_status_code(201);
			else if (request->get_method() != "POST")
			{
				if (is_page_not_found(path, loc->getRoot()))
				{
					request->set_status_code(404);
					set_error_page(default_err);
				}
			}
		}
		else
		{
			set_error_page("/405.html");
		}
	}
	else
	{
		request->set_status_code(404);
		set_error_page(default_err);
	}
}

void Response::set_error_page(std::string file)
{
	path = config->getErrorPage(request->get_hostname());
	if (path.empty())
		path = default_error;
	path.append(file);
}

void Response::handle_delete_request(void)
{
	if (request->get_status_code() != 200 || get_path().find("delete/") == std::string::npos)
	{
		return ;
	}
	if( std::remove( get_path().c_str()) != 0 )
	{
		set_error_page(default_err);
	}
}

void Response::set_content_type(void)
{
    if (is_text_ext(file_ext))
    {
       set_value("Content-type:", "text/" + ((file_ext != "php" && file_ext != "js" && file_ext != "txt") ? file_ext : (file_ext == "php" ? "html" : (file_ext == "txt" ? "plain" : "javascript"))));
    }
    else if (is_image_ext(file_ext))
    {
        set_value("Content-type:", "image/" + ((file_ext != "svg" && file_ext != "jpg") ? file_ext : (file_ext == "svg" ? "svg+xml" : "jpeg")));

    }
}

void Response::set_headers(void)
{
	set_content_type();

	if ((request->get_method() == "GET"))
	{
		set_value("Content-length:", ft::to_string(body.length()));
		set_value("Connection:", "keep-alive");
	}
	else
	{
		set_value("Connection:", "close");
		if (request->is_chunked() && request->get_status_code() < 400)
		{
			set_value("content-type:", "text/html; charset=utf-8");
			set_value("Transfer-Encoding:", "chunked");
		}
	}

    set_value("Content-security-policy:", "upgrade-insecure-requests");
    set_value("Server:", "Webserv");
    set_value("Accept-ranges:", "bytes");
    set_value("Date:", get_http_time());
}

void Response::set_image_body(void)
{
	std::ifstream in(path, std::ios::binary);
	if (!in.is_open())
	{
		stop_writing();
		return ;
	}
	else
	{
		in.seekg(0, std::ios::end);
		std::size_t length = in.tellg();
		in.seekg(0, std::ios::beg);

		if (in.fail())
		{
			in.close();
			stop_writing();
			return ;
		}
		else if (length > 0)
		{
			char buffer[1024];
			do
			{
				if (!in.read(buffer, std::min(length, sizeof(buffer))))
				{
					break;
				}
				int bytes = in.gcount();
				std::string tmp_string(std::min(length, sizeof(buffer)), 1);
				memmove(&tmp_string[0], buffer, std::min(length, sizeof(buffer)));
				body.append(tmp_string);
				length -= bytes;
			} while (length > 0);
		}
	}
}

void Response::set_body(void)
{
	if (input_stream.is_open())
		input_stream.close();
	if (file_ext == "php" && request->get_method() == "GET")
	{
		set_php_body();
	}
	else if (is_image_ext(file_ext))
	{
		set_image_body();
	}
	else if (request->get_method() == "GET" || request->get_method() == "UNKNOWN" || request->get_method() == "not found" || (request->get_method() == "POST" && !request->is_chunked()))
	{
		set_text_body();
	}
}

void Response::set_php_body(void)
{
	int pipefd[2];
	int pid;
	pipe(pipefd);
	pid = fork();
	if (pid == -1)
		stop_writing();
	else if (pid == 0)
	{
		close(pipefd[0]);
		char *test[3];
		test[0] = (char *)"php";
		test[1] = &(path.substr(path.find_last_of("/") + 1))[0];
		test[2] = NULL;
		chdir(path.erase(path.find_last_of("/")).c_str());
		dup2(pipefd[1], STDOUT_FILENO);
		if (execvp("php", test))
		{
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		int len;
		char *buffer;
		size_t n;
		buffer = NULL;
		len = 0;
		n = 0;
		if (close(pipefd[1]) != 0)
		{
			std::cout << "close1" << std::endl;
			exit(EXIT_FAILURE);
		}
		waitpid(pid, NULL, 0);

		FILE *data = fdopen(pipefd[0], "r");
		if (data == NULL)
		{
			stop_writing();
		}
		while (data && ((len = getline(&buffer, &n, data)) != -1))
		{
			body += buffer;
			free(buffer);
			buffer = NULL;
			n = 0;
		}
		fclose(data);
	}
}

void Response::set_text_body(void)
{
	std::string line;
	input_stream.open(path.c_str());
	if (input_stream.is_open())
	{
		while (std::getline(input_stream, line))
		{
			body += line.c_str();
		}
		input_stream.close();
	}
}

// -----------------GETTERS---------------- //

std::string Response::get_http_response(std::map<int,std::string>& status_line_map)
{
	set_first_line("HTTP/1.1 " + ft::itos(request->get_status_code()) + " " + status_line_map.find(request->get_status_code())->second );
	std::string response = get_http_header();
	response.append(body);
	return (response);
}

std::string Response::get_http_time()
{
	static const std::string wday_name[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const std::string mon_name[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	time_t rawtime = time(NULL);
	struct tm *timeptr = gmtime(&rawtime);

	std::stringstream date;
	date << wday_name[timeptr->tm_wday]
		 << ", " << timeptr->tm_mday << " "
		 << mon_name[timeptr->tm_mon] << " "
		 << 1900 + timeptr->tm_year << " "
		 << timeptr->tm_hour << ":"
		 << timeptr->tm_min << ":"
		 << timeptr->tm_sec << " GMT";
	return (date.str());
}

std::string Response::get_path(void)
{
	return (path);
}

// -----------------CHECKERS--------------- //

bool exists_path(std::string const path)
{
	std::ifstream exists(path.c_str());
	exists.seekg(0, std::ios::end);
	if (exists.good())
	{
		return (true);
	}
	return (false);
}

bool exists_dir(std::string const path)
{
	struct stat s;
	if( stat(path.c_str(), &s) == 0 )
	{
		if( s.st_mode & S_IFDIR )
		{
			return (true);
		}
		else if( s.st_mode & S_IFREG )
		{
			return (false);
		}
		else
		{
			std::cout << "error exists_path" << std::endl;
			return(false);
		}
	}
	else
	{
		std::cout << "error exists_path" << std::endl;
		return(false);
	}
}

bool is_image_ext(std::string ext)
{
    const std::string image[] = {"png", "jpg", "jpeg", "svg", "webp", "gif", "tiff", "avif", "bmp"};
    return (ft::is_found(image, ext, 9));
}

bool is_text_ext(std::string ext)
{
    const std::string text[] = {"html", "css", "js", "txt", "php", "xml", "csv"};
    return (ft::is_found(text, ext, 7));
}

bool is_authorized(std::string server, std::string request, bool listing_status)
{
	std::transform(server.begin(), server.end(), server.begin(), ft::to_lower);
	std::transform(request.begin(), request.end(), request.begin(), ft::to_lower);

    if (listing_status || server.find(request, 0) == std::string::npos || !is_method_valid(request))
        return (false);
    return (true);
}

bool is_method_valid(std::string method)
{
	const std::string methods[] = {"post", "head", "get", "put", "delete"};
    return (ft::is_found(methods, method, 5));
}

bool is_page_not_found(std::string path, std::string root)
{
	bool is_path_valid = exists_path(path);
	bool is_dir_valid = exists_dir(path);
	if (((is_dir_valid && path != root) || is_path_valid == false))
		return (true);
	return (false);
}

bool is_redirection(std::string path)
{
	if (path.length() < 12)
		return (false);
	if (path.substr(0, 12) == "/redirection")
		return (true);
	return (false);
}