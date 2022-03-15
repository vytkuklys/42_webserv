#include "Config.hpp"

Config::Config(std::string inArgv1) : filename(inArgv1) {}

Config::~Config(void) {}

void Config::pushContainers(int level)
{
	if (level == 1)
	{
		if (ft::stoi(sPort) == 0)
			std::cout << "Invalid port: " << sPort << std::endl;
		vPorts.push_back(ft::stoi(sPort));
		sPort.erase();
	}
	if (level == 2)
	{
		vServerNames.push_back(serverName);
		serverName.erase();
	}
	if (level == 3)
	{
		vErrorPages.push_back(errorPage);
		errorPage.erase();
	}
	if (level == 4)
	{

		if (ft::stoi(sBodySize) > 10)
		{
			std::cout << "client_max_body_size is bigger than 10M" << std::endl;
			vBodySizes.push_back(10);
		}
		else
			vBodySizes.push_back(ft::stoi(sBodySize));
		sBodySize.erase();
	}
}

void Config::setData(std::string readLine, std::string find, int level)
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
		if (level == 1)
			sPort.push_back(readLine[begin + i]);
		if (level == 2)
			serverName.push_back(readLine[begin + i]);
		if (level == 3)
			errorPage.push_back(readLine[begin + i]);
		if (level == 4)
			sBodySize.push_back(readLine[begin + i]);
		i++;
	}
	pushContainers(level);
}

int	Config::countServer(void)
{
	std::string readLine;
	std::ifstream readFile;

	int counter = 0;

	readFile.open(filename.c_str());
	if (readFile.is_open())
	{
		while (getline(readFile, readLine))
		{
				if (!(readLine.find("server")))
					counter += 1;
		}
	}
	return (counter);
}

void	Config::retrieveValues(void)
{
	std::string readLine;
	std::ifstream readFile;

	readFile.open(filename.c_str());
	if (readFile.is_open())
	{
		if (!countServer())
		{
			std::cout << "Wrong config file " << filename << std::endl;
			return ;
		}
		while (getline(readFile, readLine))
		{
			setData(readLine, "port", 1);
			setData(readLine, "server_name", 2);
			setData(readLine, "error_pages", 3);
			setData(readLine, "client_max_body_size", 4);
		}
	}
	else
		std::cout << "Unable to open file: " << filename << std::endl;
}

std::vector<int> Config::getPorts(void) { return(vPorts); }

std::vector<std::string> Config::getServerNames(void) { return(vServerNames); }

std::vector<std::string> Config::getErrorPages(void) { return(vErrorPages); }

std::vector<int> Config::getBodySizes(void) { return(vBodySizes); }


int ft_stoi(std::string s) 
{
    int i;
    std::istringstream(s) >> i;
    return (i);
}
