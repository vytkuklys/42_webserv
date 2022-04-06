
#include "http_header.hpp"

http_header::http_header() : notf("not found")
{
	first_http_line.resize(6);
}

http_header::~http_header()
{

}

const std::string& http_header::get_value(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator itr;
	if((itr = header_pairs.find(key)) == header_pairs.end())
		return(notf);
	return(itr->second);
}

const std::string& http_header::get_first_line_element(const size_t pos) const
{
	if(pos > first_http_line.size())
		return(notf);
	return(first_http_line[pos]);
}

void http_header::set_value(const std::string key, const std::string value)
{
	header_pairs.insert(std::pair<std::string, std::string>(key, value));
}

void http_header::set_first_line_element(const size_t pos, const std::string& value)
{
	if (pos >= first_http_line.size())
	{
		first_http_line.resize(pos + 1);

	}
	first_http_line[pos] = value;
}

std::string http_header::get_http_header(void) const
{
	std::string response;

	response = get_first_line() + "\r\n";
	// std::cout << get_first_line() << std::endl;
	for (std::map<std::string, std::string>::const_iterator it=header_pairs.begin(); it != header_pairs.end(); ++it)
	{
		 response += it->first + " " + it->second + "\r\n";
	}
	response += "\r\n";
	return (response);
}

std::string http_header::get_port() const
{
	std::string port;
	if (header_pairs.find("Host") != header_pairs.end())
	{
		std::string host = header_pairs.find("Host")->second;
		port = host.substr(host.find_last_of(':') + 1, host.length());
	}
	return (port);
}


void http_header::set_first_line(std::string line)
{
	size_t i;
	size_t j;
	i = 0;
	j = 0;
	int x;
	for(x = 0; (i = line.find(" ", j)) != std::string::npos; ++x)
	{
		set_first_line_element(x, line.substr(j, i - j));
		j = i + 1;
	}
	set_first_line_element(x, line.substr(j, line.size()));
}


std::string	http_header::get_first_line() const
{
	std::string ret;

	for(size_t i = 0; i < first_http_line.size(); ++i)
	{
		ret += get_first_line_element(i);
		ret += " ";
	}
	ret = (ret.substr(0, ret.length() - 1));
	return(ret);
}
