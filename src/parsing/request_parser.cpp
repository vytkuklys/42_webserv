#include "request_parser.hpp"

int Parsing::get_start_line(std::string s)
{
    size_t pos = 0;
    std::string token;
    int j = 0;

    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end()); //remove the newlines
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
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end()); //remove the newlines
    if (line.empty() || is_whitespace(line) == EXIT_SUCCESS)
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


size_t ft_stoi1(std::string s) 
{
    size_t i;
    std::istringstream(s) >> i;
    return (i);
}

std::string first_numberstring(std::string const & str)
{
  char const* digits = "0123456789";
  std::size_t const n = str.find_first_of(digits);
  if (n != std::string::npos)
  {
    std::size_t const m = str.find_first_not_of(digits, n);
    return str.substr(n, m != std::string::npos ? m-n : m);
  }
  return std::string();
}

Parsing::Parsing(int fd)
{
    char*	buffer;
    // char       tmp[10001];
    buffer = NULL;
    size_t n;
    // size_t n_read;
    FILE* data= fdopen(fd, "r");
    std::string line;
    while (data && getline(&buffer, &n, data) && get_start_line(buffer))
    {
        free(buffer);
        buffer = NULL;
    }
    free(buffer);
    buffer = NULL;
    while (data && getline(&buffer, &n, data) && get_headers(buffer) == EXIT_SUCCESS)
    {
        free(buffer);
        buffer = NULL;       
    }
    free(buffer);
    buffer = NULL;
    for_testing_print_request_struct();
    std::string boundary = first_numberstring(headers.find("Content-Type")->second);
    if (method == "POST")
    {
        std::ofstream ofs("test.txt");
        n = ft_stoi1(headers.find("Content-Length")->second);
        body.resize(n);
        fread((void *)body.c_str(),  sizeof(char), n, data);
        // while(n > 0)        // im  not shure if Content-Length has alwys the right length so maybe we hav to buffer
        // {
            // if(n > 10000)
            // {
                // n_read = 10000;
                // n = n - n_read;
            // }
            // else
            // {
                // n_read = n;
                // n = 0;
            // }
            // if (fread(tmp, sizeof(char), n_read, data) != n_read)
                // std::cout << "===========error fread===================\n";
            // tmp[n_read] = '\0';
            // body.append(tmp);
            // if(n <= 10000 && body.find(boundary.c_str()) != std::string::npos)
            // {
                // std::cout << "begin" << &body[body.find(boundary.c_str())] << " = " << boundary << std::endl;
                // break;
            // }
        // }
        ofs << body;
        ofs.close();
    }
    //fclose(data); // dosent work maybe someone wave an idear
}

std::string Parsing::get(std::string key_word)
{
    std::map<std::string, std::string>::iterator itr;
    if(key_word == "method")
        return(method);
    else if(key_word == "path")
        return(path);
    else if(key_word == "protocol")
        return(protocol);
    else
    {
        itr = headers.find(key_word);
        if(itr == headers.end())
            return(key_word);
        else
            return(std::string());
    }

}

