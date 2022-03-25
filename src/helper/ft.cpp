
#include "ft.hpp"
#include <iostream>

std::string ft::to_string(std::size_t number)
{
	std::ostringstream s;
	s << number;
	return(s.str());
}

size_t ft::stoi(std::string s)
{
	size_t i;
	std::istringstream(s) >> i;
	return (i);
}

int	ft::Str_to_Hex_to_Int(std::string s)
{
	int ret;
	std::istringstream(s) >> std::hex >> ret;
	return(ret);
}


std::string ft::first_numberstring(std::string const & str)
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

void ft::nulify(char **buffer, size_t *n)
{
	free(*buffer);
	*buffer = NULL;
	*n = 0;
}

int ft::is_whitespace(std::string line)
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

bool ft::is_found(std::string const array[], std::string target){
	int size = array->length();
	for(int counter = 0;counter<size;counter++){
		if(array[counter].compare(target) == 0){
			return true;
		}
	}
	return (false);
}

std::string ft::removeSpacesAfter(const std::string & inputString)
{
  int i = inputString.size();

  while (i >= 0 && inputString[i - 1] == ' ')
	--i;

  return (inputString.substr(0, i));
}

bool ft::replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if(start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

char ft::to_lower(char c) {
	if (c <= 'Z' && c >= 'A')
		return c - ('Z' - 'z');
	return c;
}