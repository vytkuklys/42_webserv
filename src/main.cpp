#include "../inc/Server.hpp"
#include "../inc/Configuration.hpp"

void ft_printeror(int a)
{
	(void)a;
	std::cout << "broken pipe" << std::endl;
}

bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

int main (int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Invalid input" << std::endl;
		return (-1);
	}
	if (!is_file_exist(argv[1]))
	{
		std::cout << "Invalid config file" << std::endl;
		return (-1);
	}
	signal(SIGPIPE, ft_printeror);
	Config *config = new Config(argv[1]);
	ft::doublePortChecker(*config);

	ft::displayTimestamp();
	std::cout << "Configuration file: " << argv[1] << std::endl;

	SERVER::WebServer webServer(*config);

	return (0);
}
