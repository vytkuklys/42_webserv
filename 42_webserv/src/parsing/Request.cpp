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
    int len;
    FILE* data= fdopen(fd, "r");
    std::stringstream head;
    while (data && (len = getline(&buffer, &n, data)) && set_start_line(buffer))
    {
        // std::cout << buffer << "size=" << n << " len=" << len << std::endl;
        // head << buffer;
        free(buffer);
        buffer = NULL;
        n = 0;
    }
    // head << buffer;
    free(buffer);
    buffer = NULL;
    n = 0;
    while (data && (len = getline(&buffer, &n, data)) && set_headers(buffer) == EXIT_SUCCESS)
    {
        head << buffer;
        // std::cout << buffer << "size=" << n << " len=" << len << std::endl;
        free(buffer);
        buffer = NULL;    
        n = 0; 
    }
    // head << buffer;
    // std::cout << buffer << std::endl;
    free(buffer);
    buffer = NULL;
    n = 0;
    for_testing_print_request_struct();
    if (method == "POST")
    {   
        std::cout << "post" << std::endl;
        buffer = NULL;
        n = 0;
        int pipefd_in[2];
        int pipefd_out[2];
        
        if((pipe(pipefd_in) == -1) || (pipe(pipefd_out) == -1))
        {
            std::cout << "pipe error" << std::endl;
            exit(EXIT_FAILURE);
        }
        int pid = fork();
        if (pid == -1)
            std::cout << "eror fork" << std::endl;
        if (pid == 0)
        {
            std::cout << "child" << std::endl;
            std::string p(std::string("path=") + get("path"));
            std::string m(std::string("method=") + get("method"));
            std::string pr(std::string("protocol=") + get("protocol"));
            char *test[7];
            test[0] = (char*)"php-cgi";
            test[1] = (char*)"./documents/test.php";
            test[2] = &m[0];
            test[2] = NULL;
            // std::string exp;
            std::vector<char*> cstrings;
            int i;
            i = 0;
            while(environ[i])
            {
                cstrings.push_back(environ[i]);
                // std::cout << cstrings[i] << std::endl;
                i++;
            }
            // std::vector<std::string> strings;
            // std::map<std::string, std::string>::iterator tmp = headers.begin();
            // while ( tmp != headers.end())
            // {
            //     strings.push_back((tmp->first + "=" + tmp->second));
            //     std::transform(strings.back().begin(),strings.back().begin() + strings.back().find_first_of("="), strings.back().begin(), ::toupper);
            //     if (strings.back().find("-") != std::string::npos)
            //         strings.back()[strings.back().find("-")] = '_';
            //     std::cout << strings.back() << std::endl;
            //     cstrings.push_back(const_cast<char*>(strings[strings.size() - 1].c_str()));
            //     // if (putenv(&exp[0]))
            //     //     std::cout << "in loop child" <<  tmp->first << std::endl;

            //     // if (!getenv(&tmp->first[0]))
            //     //     std::cout << "not in env" << std::endl;
            //     // std::cout << getenv(&tmp->first[0]) << std::endl;
            //     tmp++;
            // }
            cstrings.push_back((char*)"REDIRECT_STATUS=true");
            cstrings.push_back((char*)"SCRIPT_FILENAME=./documents/test.php");
            cstrings.push_back((char*)"REQUEST_METHOD=POST");
            cstrings.push_back((char*)"GATEWAY_INTERFACE=CGI/1.1");
            std::string cont;
            std::map<std::string, std::string>::iterator tmp;
            if ((tmp = headers.find("Content-Length")) != headers.end())
            {
                cont = tmp->first + "=" + tmp->second;
                std::transform(cont.begin(),cont.begin() + cont.find_first_of("="), cont.begin(), ::toupper);
                if (cont.find("-") != std::string::npos)
                    cont[cont.find("-")] = '_';
            }
            cstrings.push_back(&cont[0]);
            cstrings.push_back(NULL);
            



            std::cout << "child execvp" << std::endl;
           
            close(pipefd_out[0]);
            close(pipefd_in[1]);
            dup2(pipefd_out[1], STDOUT_FILENO);
            close(pipefd_out[1]);
            dup2(pipefd_in[0], STDIN_FILENO);
            close(pipefd_in[0]);
            // exit(EXIT_FAILURE);
            if (execvpe("php-cgi", &test[0], &cstrings[0]))
            {
                perror("execvp");
                std::cout << "execvp error" << std::endl;
                exit(EXIT_FAILURE);
            }

        }
        sleep(8);
        close(pipefd_in[0]);
        
        std::cout << "parent" << std::endl;
        len = 0;
        std::map<std::string, std::string>::iterator tmp = headers.find("Content-Length");
        if (tmp != headers.end())
        {
            int n_byts = ft::stoi(tmp->second);
            write(1, head.str().c_str(), head.str().length());
            if (write(pipefd_in[1], head.str().c_str(), head.str().length()) == -1)
                std::cout << "error header to php" << std::endl;
            while (data && n_byts && ((len = getline(&buffer, &n, data))))
            {
                // if (write(pipefd_in[1], buffer, len) == -1)
                    // std::cout << "error write1" << std::endl;
                n_byts -= len;
                std::cout << "length = " << len << "n=" << n_byts << std::endl;
                free(buffer);
                buffer = NULL;    
                n = 0; 
            }
            close(pipefd_in[1]);
            std::cout << "parent loop ende" << len << std::endl;
        }
        // else
        // {
        //     sleep(2);
        //     unsigned int x;  
        //     std::stringstream ss;
        //     while ((len = getline(&buffer, &n, data)))
        //     {
        //         std::cout << "len = " << len << std::endl;
        //         if (len > 0 && buffer[0] == '\n')
        //         {
        //             free(buffer);
        //             buffer = NULL;
        //             n = 0;
        //             continue;
        //         }
        //         // len = getline(&buffer, &n, data);
        //         ss << std::hex << buffer;
        //         ss >> x;
        //         free(buffer);
        //         buffer = NULL;
        //         std::cout << "counged input" <<  buffer << len << " x="<< x << std::endl;
        //         if (x == 0)
        //         {
        //             free(buffer);
        //             buffer = NULL;
        //             close(pipefd_in[1]);

        //         }
        //         buffer = (char *)malloc(sizeof(char) * x);
        //         fread(buffer, sizeof(char), x, data);
        //         write(pipefd_in[1], buffer, x);
        //         free(buffer);
        //         buffer = NULL;
        //     }
            
        // }
        close(pipefd_out[1]);
        std::cout << "creat file" << std::endl;
        int tmp_fd = open("./documents/test.html", O_CREAT | O_WRONLY | O_TRUNC, 0777);
        char buf[11];
        while ((len = read(pipefd_out[0], buf, 10)))
        {
            // std::cout << "file=" << tmp_fd << "len=" << len << std::endl;
            // std::cout << "print in file " << std::endl;
            if (write(tmp_fd, buf, 10) == -1)
                std::cout << "fehler write" << std::endl;
        }
        close(tmp_fd);
        close(pipefd_out[0]);
        //     std::cout << "tm+fd=" << tmp_fd <<  write(tmp_fd, "hallo\n", 5) << std::endl;
        std::cout << "parent wait" << std::endl;
        if (wait(NULL) == -1)
            std::cout << "error wait" << std::endl;
        // close(tmp_fd);
        std::cout << "========================================done==========================================" << std::endl;
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