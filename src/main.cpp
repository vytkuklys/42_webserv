#include "../inc/Configuration.hpp"
#include "../inc/Parsing.hpp"
#include "../inc/Server.hpp"

int main (int argc, char **argv)
{
	if (argc != 2 && 0) // only for faster testing
	{
		std::cout << "Invalid input." << std::endl;
		return (-1);
	}
	//Config config(argv[1]);
    (void)argv;                                 // only for faster testing
    Config config("config_files/default.conf"); // only for faster testing

	config.retrieveValues();

    SERVER::WebServer webServer(AF_INET, SOCK_STREAM, 0, config.getPorts(), INADDR_ANY, 10);

	return (0);
}
