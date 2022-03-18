#include "Config.hpp"

Config::Config(std::string inArgv1) : filename(inArgv1), npos(-1) { retrieveValues(); }

Config::~Config(void) 
{ 
	std::vector<ConfigData *>::iterator it = ContConfigData.begin();
	std::vector<ConfigData *>::iterator ite = ContConfigData.end();

	while (it != ite)
	{
		delete *it;
		++it;
	}
	ContConfigData.clear();
}

void Config::pushToClass(int level, ConfigData & tempClass)
{
	if (level == 1)
	{
		if (ft::stoi(sPort) == 0)
			std::cout << "Invalid port: " << sPort << std::endl;
		tempClass.setPort(ft::stoi(sPort));
		sPort.erase();
	}
	if (level == 2)
	{
		tempClass.setServerName(serverName);
		serverName.erase();
	}
	if (level == 3)
	{
		tempClass.setErrorPage(errorPage);
		errorPage.erase();
	}
	if (level == 4)
	{
		if (ft::stoi(sBodySize) == 0)
			tempClass.setBodySize(1);
		else if (ft::stoi(sBodySize) > 10)
		{
			std::cout << "client_max_body_size is bigger than 10M" << std::endl;
			tempClass.setBodySize(10);
		}
		else
			tempClass.setBodySize(ft::stoi(sBodySize));
		sBodySize.erase();
	}
}

void Config::setData(std::string readLine, std::string find, int level, ConfigData & tempClass, int whichLine)
{
	static int whichLocation = 0;

	size_t begin = readLine.find(find);
	if (begin == npos)
		return ;

	if (level == 5)
		tempClass.retrieveValues(filename, whichLine, (whichLine + countServerLength("location", ++whichLocation) - 1));
	else
	{
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
		pushToClass(level, tempClass);
	}
}

int	Config::countServerLength(std::string const & find, int whichOne)
{
	std::string readLine;
	std::ifstream readFile;

	int MakeItZero = 0;
	int Counter = 0;
	int serverLength = 0;

	readFile.open(filename.c_str());
	if (readFile.is_open())
	{
		while (getline(readFile, readLine))
		{
			if (readLine.find(find, 0) != npos)
				Counter += 1;
			if (Counter == whichOne)
			{
				if (readLine.find('{', 0) != npos)
					MakeItZero += 1;
				if (readLine.find('}', 0) != npos)
					MakeItZero -= 1;
				serverLength += 1;
				if (MakeItZero == 0)
					return (serverLength);
			}
		}
	}
	return (-1);
}

int	Config::countElement(std::string const & Element)
{
	std::string readLine;
	std::ifstream readFile;

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
	if (
		countElement("srvr_name") != countElement("server") ||
		countElement("error_pages") != countElement("server") ||
		countElement("client_max_body_size") != countElement("server"))
		return (-1);
	return (0);
}

void	Config::retrieveValues(void)
{
	std::string readLine;
	std::ifstream readFile;
	ConfigData * tempClass = nullptr;

	// int amountOfServers = countElement("server");
	int whichServer = 0;
	int whichLine = 1;
	bool lookForNewServer = true;
	int serverLength = -1;

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
			if (lookForNewServer == true)
			{
				if (readLine.find("server") != npos)
				{
					// amountOfServers -= 1;
					// if (amountOfServers == -1)
					// 	break ;
					serverLength = countServerLength("server", ++whichServer);
					if (serverLength > 2)
					{
						tempClass = new ConfigData();
						lookForNewServer = false;
					}
				}
			}
			if (lookForNewServer == false)
			{
				setData(readLine, "port", 1, *tempClass, whichLine);
				setData(readLine, "srvr_name", 2, *tempClass, whichLine);
				setData(readLine, "error_pages", 3, *tempClass, whichLine);
				setData(readLine, "client_max_body_size", 4, *tempClass, whichLine);
				setData(readLine, "location", 5, *tempClass, whichLine);
				serverLength--;
				if (serverLength == 0)
				{
					ContConfigData.push_back(tempClass);
					lookForNewServer = true;
				}
			}
			++whichLine;
		}
	}
	else
		std::cout << "Unable to open file: " << filename << std::endl;
}

std::vector<ConfigData *> & Config::getContConfigData(void) { return(ContConfigData); }

int ft_stoi(std::string s)
{
    int i;
    std::istringstream(s) >> i;
    return (i);
}
