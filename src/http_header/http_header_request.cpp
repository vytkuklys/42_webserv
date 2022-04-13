#include "http_header_request.hpp"

http_header_request::http_header_request()
{
}

http_header_request::~http_header_request()
{
}

const std::string		http_header_request::get_method() const
{
	return (get_first_line_element(0));
}

const std::string		http_header_request::get_path() const
{
	return (get_first_line_element(1));
}

const std::string		http_header_request::get_protocol() const
{
	return (get_first_line_element(2));
}

void 		http_header_request::set_method(std::string& value)
{

	set_first_line_element(0, value);
}

void 		http_header_request::set_path(std::string& value)
{
	set_first_line_element(1, value);
}

void 		http_header_request::set_protocol(std::string& value)
{
	set_first_line_element(2, value);
}
