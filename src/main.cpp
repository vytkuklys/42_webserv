#include "../inc/Configuration.hpp"
#include "../inc/Parsing.hpp"
#include "../inc/Server.hpp"

void printConfigData(Config config)
{
	std::vector<ConfigData *> allData = config.getContConfigData();
	std::vector<ConfigData *>::iterator it = allData.begin();
	std::vector<ConfigData *>::iterator ite = allData.end();

	std::vector<LocationData *> locationData = (*it)->getContLocationData();
	std::vector<LocationData *>::iterator it2 = locationData.begin();
	std::vector<LocationData *>::iterator ite2 = locationData.end();

	while (it != ite && it2 != ite2)
	{
		std::cout << "Port: " << (*it)->getPort() << std::endl;
		std::cout << "Server Name: " << (*it)->getServerName() << std::endl;
		std::cout << "Error Page: " << (*it)->getErrorPage() << std::endl;
		std::cout << "Body Size: " << (*it)->getBodySize() << std::endl;

		std::cout << "root: " << (*it2)->getRoot() << std::endl << std::endl;

		++it;
		++it2;
	}
}

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

	printConfigData(config);

	std::vector<int> testPorts;
	testPorts.push_back(8080);
	testPorts.push_back(8085);
	testPorts.push_back(9999);

    SERVER::WebServer webServer(AF_INET, SOCK_STREAM, 0, testPorts, INADDR_ANY, 10);

	return (0);
}
