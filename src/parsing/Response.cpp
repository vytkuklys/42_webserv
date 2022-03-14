#include "Response.hpp"
#include <string.h>
#include <sys/stat.h>

Response::Response(Parsing request) : Request(request)
{
    set_path(Request.get_path());
    std::cout << "requested path: " << Request.get("path") << "\n";
    set_status_line();
    set_body();
    set_headers();
}

// ----------------- SETTERS ------------------- //

void Response::set_path(std::string const filename)
{
    path = "./documents" + filename;
    if (filename[filename.length() - 1] == '/')
        path += "index.html";
    is_path_valid = exists_path(path.c_str());
    if (!is_path_valid)
        path = "./documents/error_pages/404.html";
}

void Response::set_status_line(void)
{
    if (is_path_valid)
        status_line = "HTTP/1.1 200 OK";
    else
        status_line = "HTTP/1.1 404 NOT FOUND";
}


void Response::set_headers(void)
{
    if (path.substr(path.find_last_of('.') + 1, path.length()).compare("svg") == 0)
        headers["Content-type:"] = "image/svg+xml"; // currently only functional with text types (css, html, xml), other types: image, video, multipart..
    else
        headers["Content-type:"] = "text/" + path.substr(path.find_last_of('.') + 1, path.length()); // currently only functional with text types (css, html, xml), other types: image, video, multipart..
    headers["Content-length:"] = ft::to_string(body.length());
    headers["Content-security-policy:"] = "upgrade-insecure-requests"; // static header against XSS and data injection attacks not really neede :D
    headers["Connection:"] = "close";                                  // keep-alive value might be needed for file sending
    headers["Accept-ranges:"] = "bytes";                               // can be used somehow for file download
    headers["Date:"] = get_http_time();

    // other headers to consider: (1)Server
}

void Response::set_body(void)
{
    if (input_stream.is_open())
        input_stream.close();
    if (path.substr(path.find_last_of(".") + 1) == "php")
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
            test[0] = (char*)"php";
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
            char* buffer;
            size_t n;
            buffer = NULL;
            len = 0;
            n = 0;
            close(pipefd[1]);
            // std::cout << "parent wait" << std::endl;
            if (wait(NULL) == -1)
                std::cout << "error wait" << std::endl;
            // std::cout << "child returnd" << std::endl;
            FILE* data = fdopen(pipefd[0], "r");
            while (data && ((len = getline(&buffer, &n, data)) != -1))
            {
                // std::cout << buffer << "size=" << n << " len=" << len << std::endl;
                body += buffer;
                free(buffer);
                buffer = NULL;
                n = 0;
            }
            fclose(data);
            path.resize(path.length() - 3);
            path.append("html");
            // std::cout << "parent end" << std::endl;
        }
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
    response += body;
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

// -----------------CHECKERS--------------- //

bool exists_path(std::string const path)
{
    std::ifstream exists(path.c_str());
    if (exists.is_open())
        return (true);
    return (false);
}
