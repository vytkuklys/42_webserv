
#include "ft.hpp"

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
