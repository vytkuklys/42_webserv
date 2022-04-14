#ifndef ft_HPP
#define ft_HPP

#include <string>
#include <sstream>
#include <cstdlib>
#include <sys/time.h>

namespace ft
{
    std::string 	to_string(std::size_t number);
    size_t 			stoi(std::string s);
    std::string 	first_numberstring(std::string const & str);
    void 			nulify(char **buffer, size_t *n);
    int 			is_whitespace(std::string line);
    int 			is_found(std::string const array[], std::string target, int size);
    std::string		removeSpacesAfter(const std::string & inputString);
    std::string		remove_whitespace(std::string line);
    bool 			replace(std::string& str, const std::string& from, const std::string& to);
    char 			to_lower(char c);
	int				Str_to_Hex_to_Int(std::string s);
	std::string 	itos(int i);
	int 			get_bytes(std::string s);
	unsigned long	time(void);
	unsigned long	ft_time_dif(unsigned long value);

}
#endif
