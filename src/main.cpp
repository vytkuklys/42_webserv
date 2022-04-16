#include "../inc/Configuration.hpp"
#include "../inc/Server.hpp"

void	doublePortChecker(Config & config)
{
	std::vector<int>::iterator it = config.getPorts().begin();
	std::vector<int>::iterator ite = config.getPorts().end();
	int i = 1;
	while (it != ite)
	{
		std::vector<int>::iterator it2 = config.getPorts().begin() + i;
		std::vector<int>::iterator ite2 = config.getPorts().end();
		int temp = *it;
		while (it2 != ite2)
		{
			if (temp == *it2)
			{
				std::cout << "Invalid config file, please make sure to use different ports for each server" << std::endl;
				exit(-1);
			}
			++it2;
		}
		++i;
		++it;
	}
}

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
	doublePortChecker(*config);

	ft::displayTimestamp();
	std::cout << "Configuration file: " << argv[1] << std::endl;

	SERVER::WebServer webServer(*config);

	return (0);
}
