#include "Request.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>
int Parsing::set_start_line(std::string s)
{
    size_t pos = 0;
    std::string token;
    int j = 0;

    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end()); // remove the newlines
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

int Parsing::set_headers(std::string line)
{
    size_t pos;
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end()); // remove the newlines
    if (line.empty() || ft::is_whitespace(line) == EXIT_SUCCESS)
        return (EXIT_FAILURE);
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
    char buffer[4001];
    std::string line;
    size_t bytes = recv(fd, buffer, 4000, 0);
    std::istringstream data(std::string(buffer, bytes), std::ios::binary);
    bytes += 1;
    while (data && std::getline(data, line) && set_start_line(line))
        ;
    while (data && std::getline(data, line) && set_headers(line) == EXIT_SUCCESS)
        ;

    std::cout << "\n"
              << path << "\n";

    if (method == "POST") // check if it is a post request
    {
        if (pipe(pipefd) == -1)
            std::cout << "pipe error" << std::endl;
        int fdp = fork();
        if (fdp == -1)
            std::cout << "eror fork" << std::endl;
        if (fdp == 0)
        {
            std::string p(std::string("path=") + get("path"));
            std::string m(std::string("method=") + get("method"));
            std::string pr(std::string("protocol=") + get("protocol"));
            char *test[6];
            test[0] = (char *)"./cgi-bin/cgi.php";
            test[1] = (&p[0]);
            test[2] = (&m[0]);
            test[3] = (&pr[0]);
            std::string header;
            std::map<std::string, std::string>::iterator tmp = headers.begin();
            while (tmp != headers.end())
            {
                header.append(tmp->first + "=" + tmp->second);
                tmp++;
                if (tmp == headers.end())
                    break;
                header.append("&");
            }
            test[4] = (&header[0]);
            test[5] = NULL;
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            if (execvp("./cgi-bin/cgi.php", &test[0]))
            {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }

        close(pipefd[0]);
        if (!is_chunked())
        {
            int test = 0;
            while (data && std::getline(data, line))
            {
                write(pipefd[1], line.c_str(), line.length());
                std::cout << "Line: " << line << std::endl;
                test++;
            }
            std::cout << "loops after reading header in POST: " << test << "\n";
        }
    }
    // fclose(data); // dosent work maybe someone wave an idear

    // fclose also closes file descriptor. How to
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

int Parsing::get_content_length()
{
    std::map<std::string, std::string>::iterator tmp = headers.find("Content-Length");
    if (tmp != headers.end())
        return (ft::stoi(tmp->second));
    return (-1);
}

// --------------------- CHECKERS ---------------------

bool Parsing::is_chunked(void)
{
    if (headers.find("Content-Length") == headers.end())
    {
        std::cout << "Chunked branch\n";
        return (true);
    }
    return (false);
}

// --------------- OVERLOADS ---------------- //

void Parsing::set_regular_body(int fd)
{
    if (fd == 7)
    {
        std::cout << "Is this the case?";
    }
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
    size_t written = write(pipefd[1], data.str().c_str(), bytes);
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
        close(pipefd[1]);
        wait(NULL);
    }
}

void Parsing::set_chunked_body(int fd)
{

    // handle chunked below
    fd++;
    //  unsigned int x;
    //             std::stringstream ss;
    //             while ((len = getline(&buffer, &n, data)))
    //             {
    //                 std::cout << "len = " << len << std::endl;
    //                 if (len > 0 && buffer[0] == '\n')
    //                 {
    //                     free(buffer);
    //                     buffer = NULL;
    //                     n = 0;
    //                     continue;
    //                 }
    //                 // len = getline(&buffer, &n, data);
    //                 ss << std::hex << buffer;
    //                 ss >> x;
    //                 free(buffer);
    //                 buffer = NULL;
    //                 std::cout << "counged input" << buffer << len << " x=" << x << std::endl;
    //                 if (x == 0)
    //                 {
    //                     free(buffer);
    //                     buffer = NULL;
    //                     close(pipefd[1]);
    //                 }
    //                 buffer = (char *)malloc(sizeof(char) * x);
    //                 fread(buffer, sizeof(char), x, data);
    //                 write(pipefd[1], buffer, x);
    //                 free(buffer);
    //                 buffer = NULL;
    //             }
}
