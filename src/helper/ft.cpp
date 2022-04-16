
#include "ft.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

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

std::string ft::itos(int i)
{
	std::stringstream ss;
	ss << i;
	return(ss.str());
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

std::string ft::remove_whitespace(std::string line)
{
    int i = line.size();

    while (i >= 0 && std::isspace(line[i - 1]))
        --i;
    return (line.substr(0, i));
}

int ft::is_found(std::string const array[], std::string target, int size)
{
	if (target.empty()) return (0);

	std::transform(target.begin(), target.end(), target.begin(), to_lower);
    for (int counter = 0; counter < size; counter++)
    {
		std::string item = array[counter];
		std::transform(item.begin(), item.end(), item.begin(), to_lower);
        if (item.compare(target) == 0)
        {
            return counter + 1;
        }
    }
    return (0);
}

std::string ft::removeSpacesAfter(const std::string &inputString)
{
    int i = inputString.size();

  while (i >= 0 && inputString[i - 1] == ' ')
	--i;

  return (inputString.substr(0, i));
}

bool ft::replace(std::string &str, const std::string &from, const std::string &to)
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

int ft::get_bytes(std::string s)
{
	const std::string units[] = {"B", "K", "KB", "M", "MB"};

	int i = 0;
	while (isdigit(s[i++]));

	int unit = ft::is_found(units, s.substr(i - 1, s.length() - 1), 5);

	switch(unit) {
	case 1:
		return ft::stoi(s);

	case 2: case 3:
		return ft::stoi(s) * 1024;

	case 4: case 5:
		return ft::stoi(s) * 1024 * 1024;

	default:
		return 1;
	}
}

unsigned long	ft::time(void)
{
	struct timeval	t;

	gettimeofday(&t, NULL);
	return ((t.tv_sec));
}

unsigned long	ft::ft_time_dif(unsigned long value)
{
	return (ft::time() - value);
}

void	ft::displayTimestamp(void)
{
	std::time_t t = std::time(NULL);
  	std::tm *const pTInfo = std::localtime(&t);

	std::ostringstream time;
	time << std::setfill('0') << '[' << std::setw(2) << pTInfo->tm_hour << ':' << 
	std::setw(2) << pTInfo->tm_min << ":" << std::setw(2) << pTInfo->tm_sec << "] ";

	std::cout << "\x1B[1m" + (KCYN + time.str() + RST) + RST;
}

//void	doublePortChecker(Config & config)
//{
//	std::vector<int>::iterator it = config.getPorts().begin();
//	std::vector<int>::iterator ite = config.getPorts().end();
//	int i = 1;
//	while (it != ite)
//	{
//		std::vector<int>::iterator it2 = config.getPorts().begin() + i;
//		std::vector<int>::iterator ite2 = config.getPorts().end();
//		int temp = *it;
//		while (it2 != ite2)
//		{
//			if (temp == *it2)
//			{
//				std::cout << "Invalid config file, please make sure to use different ports for each server" << std::endl;
//				exit(-1);
//			}
//			++it2;
//		}
//		++i;
//		++it;
//	}
//}
