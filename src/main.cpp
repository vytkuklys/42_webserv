#include "../inc/Configuration.hpp"
#include "../inc/Parsing.hpp"
#include "../inc/Server.hpp"

int main (int argc, char **argv)
{
	if (argc != 2 && 0) // only for faster testing
	{
		std::cout << "Invalid input" << std::endl;
		return (-1);
	}
	//Config config(argv[1]);
    (void)argv;                                 // only for faster testing
    Config config("config_files/newdefault.conf"); // only for faster testing

	// std::vector<ConfigData *> allData = config.getContConfigData();
	// std::vector<ConfigData *>::iterator it = allData.begin();
	// std::vector<ConfigData *>::iterator ite = allData.end();

	// while (it != ite)
	// {
	// 	std::cout << "Port: " << (*it)->getPort() << std::endl;
	// 	std::cout << "Server Name: " << (*it)->getServerName() << std::endl;
	// 	std::cout << "Error Page: " << (*it)->getErrorPage() << std::endl;
	// 	std::cout << "Body Size: " << (*it)->getBodySize() << std::endl << std::endl;
	// 	++it;
	// }

	std::vector<int> testPorts;
	testPorts.push_back(8080);
	testPorts.push_back(8085);
	testPorts.push_back(9999);

    SERVER::WebServer webServer(AF_INET, SOCK_STREAM, 0, testPorts, INADDR_ANY, 10);

	return (0);
}
