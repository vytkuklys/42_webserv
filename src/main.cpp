#include "../inc/Configuration.hpp"
// #include "../inc/Parsing.hpp"
#include "../inc/Server.hpp"

void printConfigData(Config config)
{
	std::vector<ConfigData *> allData = config.getContConfigData();
	std::vector<ConfigData *>::iterator it = allData.begin();
	std::vector<ConfigData *>::iterator ite = allData.end();

	while (it != ite)
	{
		std::cout << std::endl << "~ * * * * SERVER * * * * ~" << std::endl;
		std::cout << "Port: '" << (*it)->getPort() << "'" << std::endl;
		std::cout << "Server Name: '" << (*it)->getServerName() << "'" << std::endl;
		std::cout << "Error Page: '" << (*it)->getErrorPage() << "'" << std::endl;
		std::cout << "Body Size: '" << (*it)->getBodySize() << "'" << std::endl;

		std::vector<LocationData *> locationData = (*it)->getContLocationData();
		std::vector<LocationData *>::iterator it2 = locationData.begin();
		std::vector<LocationData *>::iterator ite2 = locationData.end();
		while (it2 != ite2)
		{
			std::cout << std::endl << "location: '" << (*it2)->getLocation() << "'" << std::endl;
			std::cout << "root: '" << (*it2)->getRoot() << "'" << std::endl;
			std::cout << "method: '" << (*it2)->getMethod() << "'" << std::endl;
			std::cout << "index: '" << (*it2)->getIndex() << "'" << std::endl;

			++it2;
		}
		std::cout << "~ * * * *  E N D  * * * * ~" << std::endl << std::endl;

		++it;
	}
}

void ft_printeror(int a)
{
	(void)a;
	std::cout << "broken pipe" << std::endl;
	// exit(SIGPIPE);
}

int main (int argc, char **argv)
{
	if (argc != 2 && 0) // only for faster testing
	{
		std::cout << "Invalid input" << std::endl;
		return (-1);
	}
	signal(SIGPIPE, ft_printeror);
	// Config config(argv[1]);
	(void)argv;								 // only for faster testing
	Config config("config_files/test.conf"); // only for faster testing

	// printConfigData(config);

	SERVER::WebServer webServer(config.getPorts(), config); //could not understand why calling config.getPorts() in webServerMac.cpp causes errors
	return (0);
}
