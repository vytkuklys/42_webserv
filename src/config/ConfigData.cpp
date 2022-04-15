#include "ConfigData.hpp"

ConfigData::ConfigData(void) : npos(-1), Port(8080), serverName("UNKNOWN"),
errorPage("./documents/html_errors"), defaultErr("404.html"), BodySize(1), directoryListing("on") {}

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
	this->directoryListing = value.directoryListing;
	this->errorPage = value.errorPage;
	this->BodySize = value.BodySize;
	this->location = value.location;
	this->root = value.root;
	this->method = value.method;
	this->index = value.index;
	this->script = value.script;
	return *this;
}

int ConfigData::checkingForTrash()
{
    int size = method.length();
    char array[size + 1];

    for(int i = 0; i < size; i++)
	{
        array[i] = method[i];
	}
	array[size] = '\0';

	char* splitResult = strtok(array, " ");
	while (splitResult != NULL)
	{
		if (strcmp(splitResult, "post") != 0 && strcmp(splitResult, "delete") != 0 &&
			strcmp(splitResult, "get") != 0 && strcmp(splitResult, "head") != 0 &&
			strcmp(splitResult, "put") != 0)
		{
			return (1);
		}
		splitResult = strtok(NULL, " ");
	}

	return (0);
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
		if ((method.find("post") == npos && method.find("delete") == npos &&
			method.find("get") == npos && method.find("head") == npos &&
			method.find("put") == npos) || checkingForTrash() == 1 || method.length() > 24)
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
	if (level == 5)
	{
		if (script.length() == 0 || script.length() > 50)
			std::cout << "Invalid script: '" << script << "'" << std::endl;
		else
			tempClass.setScript(script);
		script.erase();
	}
	if (level == 6)
	{
		int bytes = 1;
		if (ft::stoi(max_size) == 0 || max_size.length() == 0 || max_size[0] == '-')
		{
			std::cout << "Invalid client_max_body_size: '" << max_size << "'";
			std::cout << ", default client_max_body_size: '1B' used instead" << std::endl;
			tempClass.setMaxBody(bytes);
		}
		else
		{
			bytes = ft::get_bytes(max_size);
			tempClass.setMaxBody(bytes);
		}
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
		if (readLine[readLine.length() - 1] != '{')
		{
			std::cout << "Invalid config file, missing a '{'" << std::endl;
			exit(-1);
		}
		while (readLine[begin + i] != '{')
		{
			location.push_back(readLine[begin + i]);
			i++;
		}
		pushToClass(level, tempClass);
	}
	else
	{
		if (readLine[readLine.length() - 1] != ';')
		{
			std::cout << "Invalid config file, missing a ';'" << std::endl;
			exit(-1);
		}
		while (readLine[begin + i] != ';')
		{
			if (level == 2)
				root.push_back(readLine[begin + i]);
			if (level == 3)
				method.push_back(readLine[begin + i]);
			if (level == 4)
				index.push_back(readLine[begin + i]);
			if (level == 5)
				script.push_back(readLine[begin + i]);
			if (level == 6)
				max_size.push_back(readLine[begin + i]);
			i++;
		}
		pushToClass(level, tempClass);
	}
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
			{
				tempClass = new LocationData();
					tempClass->setMaxBody(BodySize);
			}
		if (whichLine >= start && whichLine < end)
		{
			setData(readLine, "location", 1, *tempClass);
			setData(readLine, "root", 2, *tempClass);
			setData(readLine, "method", 3, *tempClass);
			setData(readLine, "indx", 4, *tempClass);
			setData(readLine, "script", 5, *tempClass);
			setData(readLine, "max_body_size", 6, *tempClass);
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

void ConfigData::setDefaultErr(std::string inputDefaultErr) { defaultErr = inputDefaultErr; }

void ConfigData::setBodySize(int inputBodySizes) { BodySize = inputBodySizes; }

void ConfigData::setDirectoryListing(std::string inputDirectoryListing) { directoryListing = inputDirectoryListing; }


int ConfigData::getPort(void) { return (Port); }

std::string ConfigData::getServerName(void) { return (serverName); }

std::string ConfigData::getErrorPage(void) { return (errorPage); }

std::string ConfigData::getDefaultErr(void) { return (defaultErr); }

int ConfigData::getBodySize(void) { return (BodySize); }

std::string ConfigData::getDirectoryListing(void) { return (directoryListing); }


std::vector<LocationData *> &ConfigData::getContLocationData(void) { return (ContLocationData); }
