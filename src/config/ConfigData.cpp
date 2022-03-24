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

ConfigData::ConfigData(const ConfigData &value) : npos(value.npos)
{
	*this = value;
}

ConfigData &ConfigData::operator=(ConfigData const &value)
{
	int size = value.ContLocationData.size();
	int i = 0;
	while (i < size)
	{
		LocationData *tempClass = new LocationData(*value.ContLocationData[i]);
		ContLocationData.push_back(tempClass);
		i++;
	}
	this->Port = value.Port;
	this->serverName = value.serverName;
	this->errorPage = value.errorPage;
	this->BodySize = value.BodySize;
	this->location = value.location;
	this->root = value.root;
	this->method = value.method;
	this->index = value.index;
	return *this;
}

void ConfigData::pushToClass(int level, LocationData &tempClass)
{
	if (level == 1)
	{
		if (location.length() == 0 || location.length() > 50)
			std::cout << "Invalid location: '" << location << "'" << std::endl;
		else
		{
			location = ft::removeSpacesAfter(location);
			tempClass.setLocation(location);
		}
		location.erase();
	}
	if (level == 2)
	{
		if (root.length() == 0 || root.length() > 50)
			std::cout << "Invalid root: '" << root << "'" << std::endl;
		else
			tempClass.setRoot(root);
		root.erase();
	}
	if (level == 3)
	{
		if (method != "post" && method != "get" &&  method != "delete" &&
			method != "post get" && method != "get post" &&
			method != "get delete" && method != "delete get" && 
			method != "post delete" && method != "delete post" && 
			method != "post get delete" && method != "post delete get" && 
			method != "get delete post" && method != "get post delete" && 
			method != "delete post get" && method != "delete get post")
				std::cout << "Invalid method: '" << method << "'" << std::endl;
		else
			tempClass.setMethod(method);
		method.erase();
	}
	if (level == 4)
	{
		if (index.length() == 0 || index.length() > 50)
			std::cout << "Invalid indx: '" << index << "'" << std::endl;
		else
			tempClass.setIndex(index);
		index.erase();
	}
}

void ConfigData::setData(std::string readLine, std::string find, int level, LocationData &tempClass)
{
	size_t begin = readLine.find(find);
	if (begin == npos)
		return;

	begin = static_cast<int>(begin) + find.length();
	int i = 0;

	while (isspace(readLine[begin + i]))
		i++;

	if (level == 1)
	{
		while (readLine[begin + i] != '{')
		{
			location.push_back(readLine[begin + i]);
			i++;
		}
	}
	else
	{
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
	}
	pushToClass(level, tempClass);
}

void ConfigData::retrieveValues(std::string const filename, int start, int end)
{
	std::string readLine;
	std::ifstream readFile;
	LocationData *tempClass = nullptr;

	int whichLine = 1;

	readFile.open(filename.c_str());
	while (std::getline(readFile, readLine))
	{
		if (whichLine == start)
			if (readLine.find("location") != npos)
				tempClass = new LocationData();
		if (whichLine >= start && whichLine < end)
		{
			setData(readLine, "location", 1, *tempClass);
			setData(readLine, "root", 2, *tempClass);
			setData(readLine, "method", 3, *tempClass);
			setData(readLine, "indx", 4, *tempClass);
		}
		++whichLine;
		if (whichLine == end)
		{
			ContLocationData.push_back(tempClass);
			break;
		}
	}
}

void ConfigData::setPort(int inputPort) { Port = inputPort; }

void ConfigData::setServerName(std::string inputServerName) { serverName = inputServerName; }

void ConfigData::setErrorPage(std::string inputErrorPages) { errorPage = inputErrorPages; }

void ConfigData::setBodySize(int inputBodySizes) { BodySize = inputBodySizes; }

int ConfigData::getPort(void) { return (Port); }

std::string ConfigData::getServerName(void) { return (serverName); }

std::string ConfigData::getErrorPage(void) { return (errorPage); }

int ConfigData::getBodySize(void) { return (BodySize); }

std::vector<LocationData *> &ConfigData::getContLocationData(void) { return (ContLocationData); }
