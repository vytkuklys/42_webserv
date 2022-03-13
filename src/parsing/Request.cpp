#include "Request.hpp"
#include <unistd.h>
#include <sys/wait.h>

int Parsing::set_start_line(std::string s)
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

int Parsing::set_headers(std::string line)
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

Parsing::Parsing(int fd)
{
    char*	buffer;
    buffer = NULL;
    size_t n;
    FILE* data= fdopen(fd, "r");
    while (data && getline(&buffer, &n, data) && set_start_line(buffer))
    {
        std::cout << buffer << "size=" << n << std::endl;
        free(buffer);
        buffer = NULL;
        n = 0;
    }
    free(buffer);
    buffer = NULL;
    n = 0;
    while (data && getline(&buffer, &n, data) && set_headers(buffer) == EXIT_SUCCESS)
    {
        std::cout << buffer << "size=" << n << std::endl;
        free(buffer);
        buffer = NULL;    
        n = 0; 
    }
    // std::cout << buffer << std::endl;
    free(buffer);
    buffer = NULL;
    n = 0;
    // for_testing_print_request_struct();
    if (method == "POST")
    {
        getline(&buffer, &n, data);
        std::cout << buffer << "size=" << n << std::endl;
        free(buffer);
        buffer = NULL;
        int fdp = fork();
        if (fdp == -1)
            std::cout << "eror fork" << std::endl;
        if (fdp == 0)
        {
            // char * test[2];
            // test[0] = (char*)"./documents/test.php";
            // test[1] = NULL;

            // char * test1[3];
            // test[0] = (char*)"test=TEST";
            // test[1] = (char*) "PATH=/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/sumo_binaries/bin:/home/tom/miniconda3/bin:/home/tom/miniconda3/condabin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin"
            // test[2] = NULL;

            std::string p(std::string("path=") + get("path"));
            std::string m(std::string("method=") + get("method"));
            std::string pr(std::string("protocol=") + get("protocol"));
            char *test[6];
            test[0] = (char*)"./documents/test.php";
            test[1] = (&p[0]);
            test[2] = (&m[0]);
            test[3] = (&pr[0]);
            std::string header;
            std::map<std::string, std::string>::iterator tmp = headers.begin();
            while ( tmp != headers.end())
            {
                header.append(tmp->first + "=" + tmp->second);
                tmp++;
                if (tmp == headers.end())
                    break;
                header.append("&");
            }
            test[4] = (&header[0]);
            test[5] = NULL;
            dup2(fd, STDIN_FILENO);
            if (execvp("./documents/test.php", &test[0]))
            {
                perror("execvp");
                exit(EXIT_FAILURE);
            }

        }
        wait(NULL);
        std::cout << "========================================done==========================================" << std::endl;
        // system("php ./cgi-bin/example.php");
        // std::ofstream ofs("test.txt");
        // n = ft::stoi(headers.find("Content-Length")->second);
        // body.resize(n);
        // fread((void *)body.c_str(),  sizeof(char), n, data);
        // std::string boundary = first_numberstring(headers.find("Content-Type")->second);
        // while(n > 0)        // im  not shur if Content-Length has alwys the right length so maybe we hav to buffer
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
        // ofs << body;
        // ofs.close();
    }
    //fclose(data); // dosent work maybe someone wave an idear
}


// ----------------- GETTERS ------------------ //

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

std::map<std::string, std::string> Parsing::get_header() const{
    return (headers);
}
std::string Parsing::get_method() const{
    return (method);
}
std::string Parsing::get_path() const{
    return (path);
}
std::string Parsing::get_protocol() const{
    return (protocol);
}
std::string Parsing::get_body() const{
    return (body);
}

// ------------------ CHECKERS ------------------ //

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