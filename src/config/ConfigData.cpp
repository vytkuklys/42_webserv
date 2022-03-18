#include "ConfigData.hpp"

ConfigData::ConfigData(void) : npos(-1), Port(8080), 
serverName("UNKNOWN"), errorPage("UNKNOWN"), BodySize(1) {}

ConfigData::~ConfigData(void) 
{ 
	std::vector<LocationData *>::iterator it = ContLocationData.begin();
	std::vector<LocationData *>::iterator ite = ContLocationData.end();

	while (it != ite)
	{
		delete *it;
		++it;
	}
	ContLocationData.clear();
}



void ConfigData::pushToClass(int level, LocationData & tempClass)
{
	if (level == 2)
	{
		tempClass.setRoot(root);
		root.erase();
	}
	if (level == 3)
	{
		tempClass.setMethod(method);
		method.erase();
	}
		if (level == 4)
	{
		tempClass.setIndex(index);
		index.erase();
	}
}

void ConfigData::setData(std::string readLine, std::string find, int level, LocationData & tempClass)
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
		if (level == 2)
			root.push_back(readLine[begin + i]);
		if (level == 3)
			method.push_back(readLine[begin + i]);
		if (level == 4)
			index.push_back(readLine[begin + i]);
		i++;
	}
	pushToClass(level, tempClass);
}

void ConfigData::retrieveValues(std::string const filename, int start, int end)
{
	std::string readLine;
	std::ifstream readFile;
	LocationData * tempClass = nullptr;

	int whichLine = 1;

	readFile.open(filename.c_str());
	while (std::getline(readFile, readLine))
	{
		if (whichLine == start)
			// if (readLine.find("location") != npos)
				tempClass = new LocationData();	
		if (whichLine >= start && whichLine < end)
		{
			setData(readLine, "root", 2, *tempClass);
			setData(readLine, "method", 3, *tempClass);
			setData(readLine, "indx", 4, *tempClass);
		}
		++whichLine;
		if (whichLine == end)
		{
			ContLocationData.push_back(tempClass);
			break ;
		}
	}
}



void ConfigData::setPort(int inputPort) { Port = inputPort; }

void ConfigData::setServerName(std::string inputServerName) { serverName = inputServerName; }

void ConfigData::setErrorPage(std::string inputErrorPages) { errorPage = inputErrorPages; }

void ConfigData::setBodySize(int inputBodySizes) { BodySize = inputBodySizes; }



int ConfigData::getPort(void) { return(Port); }

std::string ConfigData::getServerName(void) { return(serverName); }

std::string ConfigData::getErrorPage(void) { return(errorPage); }

int ConfigData::getBodySize(void) { return(BodySize); }



std::vector<LocationData *> & ConfigData::getContLocationData(void) { return(ContLocationData); }

