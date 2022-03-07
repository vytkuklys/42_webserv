#include "client_handler.hpp"

client_handler::client_handler(int fd): info(fd, buffer)
{

}


client_handler::~client_handler()
{

}