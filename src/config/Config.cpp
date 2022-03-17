#include "Config.hpp"

Config::Config(std::string inArgv1) : filename(inArgv1), npos(-1) { retrieveValues(); }

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
		if (ft::stoi(sBodySize) == 0)
			vBodySizes.push_back(1);
		else if (ft::stoi(sBodySize) > 10)
		{
			std::cout << "client_max_body_size is bigger than 10M" << std::endl;
			vBodySizes.push_back(10);
		}
		else
			vBodySizes.push_back(ft::stoi(sBodySize));
		sBodySize.erase();
	}
}

//void Config::pushConfigDataClass(int level)
//{
//	(void)level;
//}

void Config::setData(std::string readLine, std::string find, int level)
{
	size_t begin = readLine.find(find);
	if (begin == npos)
		return ;

	begin = static_cast<int>(begin) + find.length();
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
	//pushConfigDataClass(level);
}

int	Config::countServerLength(int whichServer)
{
	std::string readLine;
	std::ifstream readFile;

	// static const size_t npos = -1;
	int MakeItZero = 0;
	int serverCounter = 0;
	int serverLength = 0;

	readFile.open(filename.c_str());
	if (readFile.is_open())
	{
		while (getline(readFile, readLine))
		{
			if (readLine.find("server", 0) != npos)
				serverCounter += 1;
			if (serverCounter == whichServer)
			{
				if (readLine.find('{', 0) != npos)
					MakeItZero += 1;
				if (readLine.find('}', 0) != npos)
					MakeItZero -= 1;
				serverLength += 1;
				if (MakeItZero == 0)
					return (serverLength - 2);
			}
		}
	}
	return (-1);
}

int	Config::countElement(std::string const & Element)
{
	std::string readLine;
	std::ifstream readFile;

	// static const size_t npos = -1;
	int counter = 0;

	readFile.open(filename.c_str());
	if (readFile.is_open())
	{
		while (getline(readFile, readLine))
		{
			if (readLine.find(Element, 0) != npos)
				counter += 1;
		}
	}
	return (counter);
}

int		Config::errorChecker(void)
{
	if (!countElement("server"))
		return (-1);
	if (countElement("port") != countElement("server") ||
		countElement("s_name") != countElement("server") ||
		countElement("error_pages") != countElement("server") ||
		countElement("client_max_body_size") != countElement("server"))
		return (-1);
	return (0);
}

void	Config::retrieveValues(void)
{
	std::string readLine;
	std::ifstream readFile;

	// std::cout << countElement("server") << std::endl;
	// std::cout << countServerLength(1) << std::endl;

	int amountOfServers = countElement("server");
	int whichServer = 0;

	readFile.open(filename.c_str());
	if (readFile.is_open())
	{
		if (errorChecker() == -1)
		{
			std::cout << "Wrong config file " << filename << std::endl;
			exit(-1);
		}
		while (std::getline(readFile, readLine))
		{
			if (readLine.find("server") != npos)
			{
				ConfigData* temp = new ConfigData();
				(void)temp;
				amountOfServers -= 1;
				int serverLength = countServerLength(++whichServer);
				(void)serverLength;
			}
			setData(readLine, "port", 1);
			setData(readLine, "s_name", 2);
			setData(readLine, "error_pages", 3);
			setData(readLine, "client_max_body_size", 4);
		}
	}
	else
		std::cout << "Unable to open file: " << filename << std::endl;
}

std::vector<int> & Config::getPorts(void) { return(vPorts); }

std::vector<std::string> Config::getServerNames(void) { return(vServerNames); }

std::vector<std::string> Config::getErrorPages(void) { return(vErrorPages); }

std::vector<int> Config::getBodySizes(void) { return(vBodySizes); }


int ft_stoi(std::string s)
{
    int i;
    std::istringstream(s) >> i;
    return (i);
}
