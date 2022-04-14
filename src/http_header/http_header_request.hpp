#ifndef HTTP_HEADER_REQUEST_HPP
# define HTTP_HEADER_REQUEST_HPP

#include "http_header.hpp"

class http_header_request : public http_header
{
	public:
		http_header_request();
		~http_header_request();

		const std::string		get_method() const;
		const std::string		get_path() const;
		const std::string		get_protocol() const;

		void					set_method(std::string& value);
		void					set_path(std::string& value);
		void					set_protocol(std::string& value);
};

#endif

