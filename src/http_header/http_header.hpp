#ifndef HTTP_HEADER_HPP
# define HTTP_HEADER_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "../../inc/Colors.hpp"

class http_header
{
	public:
		typedef std::map<std::string, std::string>::iterator	iterator;

	private:
		std::map<std::string, std::string>	header_pairs;
		std::vector<std::string>			first_http_line;
		const	std::string					notf;
		bool								redirection;
	public:

		http_header();
		~http_header();
		const std::string&				get_value(const std::string& key) const;
		std::string						get_http_header(void) const;
		const std::string&				get_first_line_element(const size_t pos) const;
		std::string						get_port() const;
		std::string 					get_hostname() const;
		std::string						get_first_line() const;
		bool							get_redirection_status(void) const;
		
		void							set_value(const std::string key, const std::string value);
		void							set_first_line_element(const size_t pos, const std::string& value);
		void							set_first_line(std::string line);
		void							set_redirection_status(const bool status);

		iterator						begin();
		iterator						end();
		std::pair<std::string,std::string> operator()();


};

#endif
