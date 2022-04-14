
#include "http_header.hpp"

http_header::http_header() : notf("not found")
{
	first_http_line.resize(5);
	redirection = false;
}

http_header::~http_header(){}

bool http_header::get_redirection_status(void) const
{
	return (redirection);
}

void http_header::set_redirection_status(const bool status)
{
	redirection = status;
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
	if(pos > first_http_line.size() || first_http_line[pos].empty())
	{
		return(notf);
	}
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

std::string http_header::get_http_header(void) const
{
	std::string response;

	if (get_redirection_status())
	{
		response = "HTTP/1.1 301 Moved Permanently\r\n";
		response += "Location: /\r\n\r\n";
		return (response);
	}
	response = get_first_line() + "\r\n";
	std::cout << get_first_line() << std::endl;
	for (std::map<std::string, std::string>::const_iterator it=header_pairs.begin(); it != header_pairs.end(); ++it)
	{
		 response += it->first + " " + it->second + "\r\n";
	}
	response += "\r\n";
	return (response);
}

std::string http_header::get_port() const
{
	std::string port = "";
	if (header_pairs.find("Host") != header_pairs.end())
	{
		std::string host = header_pairs.find("Host")->second;
		if (host.find_last_of(':') == std::string::npos)
			return (port);
		port = host.substr(host.find_last_of(':') + 1, host.length());
	}
	return (port);
}

std::string http_header::get_hostname() const
{
	std::string hostname;
	if (header_pairs.find("Host") != header_pairs.end())
	{
		std::string host = header_pairs.find("Host")->second;
		hostname = host.substr(0, host.find_last_of(':'));
	}
	return (hostname);
}

std::string	http_header::get_first_line() const
{
	std::string ret;

	for(size_t i = 0; i < first_http_line.size(); ++i)
	{
		if(get_first_line_element(i) != notf)
		{
			ret += get_first_line_element(i);
			ret += " ";
		}
	}
	ret = (ret.substr(0, ret.length() - 1));
	return(ret);
}
http_header::iterator	http_header::begin()
{
	return(header_pairs.begin());
}

http_header::iterator	http_header::end()
{
	return(header_pairs.end());
}
