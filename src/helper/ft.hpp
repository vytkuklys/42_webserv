#ifndef ft_HPP
#define ft_HPP

#include <string>
#include <sstream>

namespace ft
{
    std::string to_string(std::size_t number);
    size_t stoi(std::string s);
    std::string first_numberstring(std::string const & str);
    void nulify(char **buffer, size_t *n);
    int is_whitespace(std::string line);
    bool is_found(std::string const array[], std::string target);
}
#endif