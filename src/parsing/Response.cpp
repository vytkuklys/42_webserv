#include "Response.hpp"
#include <string.h>
#include <sys/stat.h>

Response::Response(Parsing request, Config data) : Request(request), config(data)
{
    default_error = "./documents/html_errors";
    set_path(Request.get_path());
    file_ext = path.substr(path.find_last_of('.') + 1, path.length());
    set_status_line();
    set_body();
    set_headers();
}

// ----------------- SETTERS ------------------- //

void Response::set_path(std::string const filename)
{
    path = filename;
    LocationData * loc = config.get_location("yes.com", filename);
    has_access = is_authorized(loc->getMethod(), Request.get_method());
    if (loc != nullptr)
	{
        if (loc->getLocation() == "/" || (path.find_last_of("/") == 0 && path.find_last_of(".") != std::string::npos))
            path.insert(0, loc->getRoot());
        else
            ft::replace(path, loc->getLocation(), loc->getRoot());
        if (filename[filename.length() - 1] == '/')
            path += loc->getIndex();
    }
    is_path_valid = exists_path(path.c_str());
    if (!is_path_valid || !has_access || loc == nullptr)
    {
        path = config.getErrorPage("yes.com");
        if (path.empty())
            path = default_error;
        if (!is_path_valid || loc == nullptr)
            path.append("/404.html");
        else
            path.append("/403.html");
    }
}

void Response::set_status_line(void)
{
    if (!is_path_valid)
        status_line = "HTTP/1.1 404 NOT FOUND";
    else if (has_access == false)
        status_line = "HTTP/1.1 403 FORBIDDEN";
    else
        status_line = "HTTP/1.1 200 OK";
}

void Response::set_content_type(void)
{
    if (is_text_ext(file_ext))
        headers["Content-type:"] = "text/" + ((file_ext != "php" && file_ext != "js" && file_ext != "txt") ? file_ext : (file_ext == "php" ? "html" : (file_ext == "txt" ? "plain" : "javascript")));
    else if (is_image_ext(file_ext))
        headers["Content-type:"] = "image/" + ((file_ext != "svg" && file_ext != "jpg") ? file_ext : (file_ext == "svg" ? "svg+xml" : "jpeg"));
}

void Response::set_headers(void)
{
    set_content_type();
    headers["Content-length:"] = ft::to_string(body.length());
    headers["Content-security-policy:"] = "upgrade-insecure-requests";
    headers["Connection:"] = "close";
    headers["Accept-ranges:"] = "bytes";
    headers["Date:"] = get_http_time();
}

void Response::set_image_body(void)
{
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open())
    {
        std::cout << "failed to open file" << std::endl;
    }
    else
    {
        in.seekg(0, std::ios::end);
        std::size_t length = in.tellg();
        in.seekg(0, std::ios::beg);

        if (in.fail())
        {
            std::cout << "failed to get size of file" << std::endl;
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
    if (file_ext == "php----------------------")
    {
        int pipefd[2];
        int pid;
        pipe(pipefd);
        pid = fork();
        if (pid == -1)
            std::cout << "error fork" << std::endl;
        if (pid == 0)
        {
            // std::cout << "child" << std::endl;
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
            // std::cout << "parent" << std::endl;
            int len;
            char *buffer;
            size_t n;
            buffer = NULL;
            len = 0;
            n = 0;
            close(pipefd[1]);
            // std::cout << "parent wait" << std::endl;
            if (wait(NULL) == -1)
                std::cout << "error wait" << std::endl;
            // std::cout << "child returnd" << std::endl;
            FILE *data = fdopen(pipefd[0], "r");
            while (data && ((len = getline(&buffer, &n, data)) != -1))
            {
                // std::cout << buffer << "size=" << n << " len=" << len << std::endl;
                body += buffer;
                free(buffer);
                buffer = NULL;
                n = 0;
            }
            fclose(data);
            // std::cout << "parent end" << std::endl;
        }
    }
    else if (is_image_ext(file_ext))
    {
        set_image_body();
    }
    else
    {
        input_stream.open(path.c_str());
        if (input_stream.is_open())
        {
            std::string line;
            while (std::getline(input_stream, line))
            {
                body += line.c_str();
            }
            input_stream.close();
        }
    }
}

// -----------------GETTERS---------------- //

std::string Response::get_http_response(void)
{
    std::string response;
    std::map<std::string, std::string>::iterator it;

    response = status_line + "\r\n";
    for (it = headers.begin(); it != headers.end(); ++it)
    {
        response += it->first + it->second + "\r\n";
    }
    response += "\r\n";
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
    if (exists.is_open())
        return (true);
    return (false);
}

bool is_image_ext(std::string ext)
{
    static const std::string image[] = {"png", "jpg", "jpeg", "svg", "webp", "gif", "tiff", "avif", "bmp"};
    return (ft::is_found(image, ext));
}

bool is_text_ext(std::string ext)
{
    static const std::string text[] = {"html", "css", "js", "txt", "php", "xml", "csv"};
    return (ft::is_found(text, ext));
}

bool is_authorized(std::string server, std::string request)
{
    std::transform(server.begin(), server.end(), server.begin(), ft::to_lower);
    std::transform(request.begin(), request.end(), request.begin(), ft::to_lower);

    std::cout << "\n server:" << server << ", req: " << request << "\n";
    if (server.find(request, 0) == std::string::npos)
        return (false);
    return (true);
}
