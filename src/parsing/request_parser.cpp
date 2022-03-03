#include "request_parser.hpp"

int get_start_line(std::string s, t_request *request)
{
    size_t pos = 0;
    std::string token;
    int j = 0;

    if (s[0] == '/' || s[0] == '#')
        return (1);
    while ((pos = s.find(" ")) != std::string::npos)
    {
        token = s.substr(0, pos);
        if ((token.length()) > 0 && j == 0)
        {
            request->method = token;
            j++;
        }
        else if (token.length() > 0 && j++ == 1)
            request->path = token;
        s.erase(0, pos + 1);
    }
    request->protocol = s;
    return (0);
}

int is_whitespace(std::string line)
{
    // int spaces = 0;
    std::size_t spaces = 0;
    for (std::string::iterator it = line.begin(); it != line.end(); ++it)
    {
        if (isspace(*it))
            spaces++;
        else
            return (EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

int get_headers(std::string line, t_request *request)
{
    size_t pos;

    if (line.empty() || is_whitespace(line) == EXIT_SUCCESS)
        return (EXIT_FAILURE);
    pos = line.find(":");
    if (!(line[0] == '/' || line[0] == '#'))
        request->headers[line.substr(0, pos)] = line.substr(pos + 2, line.length());
    return (EXIT_SUCCESS);
}

int get_body(std::string line, t_request *request)
{
    if (line.empty() || is_whitespace(line) == EXIT_SUCCESS)
        return (EXIT_SUCCESS);
    request->body.push_back(line);
    return (EXIT_SUCCESS);
}

void for_testing_print_request_struct(t_request *request)
{
    std::cout << "\n---START LINE \n";

    std::cout << request->method << " " << request->path << " " << request->protocol << "\n";
    std::map<std::string, std::string>::iterator it;
    std::cout << "\n---HEADERS \n";
    for (it = request->headers.begin(); it != request->headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << '\n';
    }
    std::vector<std::string>::iterator it2;
    std::cout << "\n---BODY \n";
    for (it2 = request->body.begin(); it2 != request->body.end(); ++it2)
    {
        std::cout << *it2 << "\n";
    }
}

int parse_request(t_request *request, int fd, char* buffer)
{
    std::string line;
    // size_t bytesRead = recv(fd, buffer, 1500, 0);
    recv(fd, buffer, 1500, 0);
    std::istringstream data(buffer);

    while (data && std::getline(data, line) && get_start_line(line, request))
        ;
    while (data && std::getline(data, line) && get_headers(line, request) == EXIT_SUCCESS)
        ;
    while (data && get_body(line, request) == EXIT_SUCCESS)
        std::getline(data, line);
    for_testing_print_request_struct(request);
    return (1);
}
