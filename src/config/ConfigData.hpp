#ifndef ConfigData_HPP
#define ConfigData_HPP

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <vector>

#include "../../inc/Helper.hpp"
#include "LocationData.hpp"

class ConfigData
{
private:
	std::vector<LocationData *> ContLocationData;

	size_t const npos;

	int Port;
	std::string serverName;
	std::string errorPage;
	int BodySize;
	std::string directoryListing;

	std::string location;
	std::string root;
	std::string method;
	std::string index;
	std::string script;
	std::string max_size;

public:
	ConfigData(void);
	~ConfigData(void);
	ConfigData(const ConfigData &value);
	ConfigData &operator=(ConfigData const &value);

	void retrieveValues(std::string const filename, int start, int end);
	void setData(std::string readLine, std::string find, int level, LocationData &tempClass);
	void pushToClass(int level, LocationData &tempClass);
	int	checkingForTrash();

	void setPort(int inputPort);
	void setServerName(std::string inputServerName);
	void setErrorPage(std::string inputErrorPages);
	void setBodySize(int inputBodySizes);
	void setDirectoryListing(std::string inputDirectoryListing);

	int getPort(void);
	std::string getServerName(void);
	std::string getErrorPage(void);
	int getBodySize(void);
	std::string getDirectoryListing(void);

	std::vector<LocationData *> &getContLocationData(void);
};

#endif
