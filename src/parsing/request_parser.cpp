#include "request_parser.hpp"

int Parsing::get_start_line(std::string s)
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
            this->method = token;
            j++;
        }
        else if (token.length() > 0 && j++ == 1)
            this->path = token;
        s.erase(0, pos + 1);
    }
    this->protocol = s;
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

int Parsing::get_headers(std::string line)
{
    size_t pos;

    if (line.empty() || is_whitespace(line) == EXIT_SUCCESS)
        return (EXIT_FAILURE);
    pos = line.find(":");
    if (!(line[0] == '/' || line[0] == '#'))
        headers[line.substr(0, pos)] = line.substr(pos + 2, line.length());
    return (EXIT_SUCCESS);
}

int Parsing::get_body(std::string line)
{
    if (line.empty() || is_whitespace(line) == EXIT_SUCCESS)
        return (EXIT_SUCCESS);
    body.push_back(line);
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
    std::vector<std::string>::iterator it2;
    std::cout << "\n---BODY \n";
    for (it2 = this->body.begin(); it2 != this->body.end(); ++it2)
    {
        std::cout << *it2 << "\n";
    }
}

Parsing::Parsing(int fd, char* buffer)
{
    std::string line;
    recv(fd, buffer, 1500, 0);
    std::istringstream data(buffer);

    while (data && std::getline(data, line) && get_start_line(line))
        ;
    while (data && std::getline(data, line) && get_headers(line) == EXIT_SUCCESS)
        ;
    while (data && get_body(line) == EXIT_SUCCESS)
        std::getline(data, line);
    for_testing_print_request_struct();
}
