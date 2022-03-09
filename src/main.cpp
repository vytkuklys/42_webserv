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
	// Config config(argv[1]);
    (void)argv;                                 // only for faster testing
    Config config("config_files/default.conf"); // only for faster testing

	config.retrieveValues();
	std::cout << config.getPort() << std::endl;
	std::cout << config.getServerName() << std::endl;
	std::cout << config.getErrorPage() << std::endl;
	std::cout << config.getBodySize() << std::endl;

    SERVER::WebServer webServer(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10);

	return (0);
}
