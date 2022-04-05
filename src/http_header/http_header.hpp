#ifndef HTTP_HEADER_HPP
# define HTTP_HEADER_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>

class http_header
{
	private:
		std::map<std::string, std::string>	header_pairs;
		std::vector<std::string>			first_http_line;
		const	std::string					notf;
	public:

		http_header();
		~http_header();
		const std::string&				get_value(const std::string& key) const;
		std::string						get_http_header(void) const;
		const std::string&				get_first_line_element(const size_t pos) const;
		std::string						get_port() const;
		std::string						get_first_line() const;
		void							set_value(const std::string key, const std::string value);
		void							set_first_line_element(const size_t pos, const std::string& value);
		void							set_first_line(std::string line);
};



#endif