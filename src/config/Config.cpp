#include "Config.hpp"

Config::Config(std::string inArgv1) : filename(inArgv1) {}

Config::~Config(void) {}

void Config::set_data(std::string readLine, std::string find)
{
	int begin = readLine.find(find);
	if (begin == -1)
		return ;
	
	begin = begin + find.length();
	int i = 0;

	while (isspace(readLine[begin + i]))
		i++;
	
	while (readLine[begin + i] != ';')
	{
		sPort.push_back(readLine[begin + i]);
		i++;
	}
}

void	Config::retrieveValues(void)
{
	std::string readLine;
	std::ifstream readFile;

	readFile.open(filename);
	if (readFile.is_open())
	{
		while (getline(readFile, readLine))
		{
			// std::cout << readLine << std::endl;
			set_data(readLine, "listen");
			// readLine = ft_replace(readLine, argv[2], argv[3]);
			// if (readFile.peek() != EOF)
		}
	}
	else
		std::cout << "Unable to open file: " << filename << std::endl;
}

std::string const Config::getServerName(void) { return(serverName); }

std::string const Config::getErrorPage(void) { return(errorPage); }

int Config::getPort(void) { return(std::stoi(sPort)); }

int	Config::getBodySize(void) { return(bodySize); }
