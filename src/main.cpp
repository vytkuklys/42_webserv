#include "../inc/Configuration.hpp"
#include "../inc/Server.hpp"

void ft_printeror(int a)
{
	(void)a;
	std::cout << "broken pipe" << std::endl;
}

int main (int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Invalid input" << std::endl;
		return (-1);
	}
	signal(SIGPIPE, ft_printeror);
	Config *config = new Config(argv[1]);

	SERVER::WebServer webServer(*config);
	return (0);
}
