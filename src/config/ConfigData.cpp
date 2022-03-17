#include "ConfigData.hpp"

ConfigData::ConfigData(void) : npos(-1), sPort(0), serverName("UNKNOWN"), 
errorPage("UNKNOWN"), sBodySize(0) {}

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



void	ConfigData::retrieveValues(std::string const filename)
{
	std::string readLine;
	std::ifstream readFile;
	LocationData * tempClass = nullptr;

	// int amountOfServers = countElement("server");
	// int whichServer = 0;
	int whichLine = 1;
	bool lookForNewServer = true;
	int serverLength = -1;

	readFile.open(filename.c_str());
	while (std::getline(readFile, readLine))
	{
		if (lookForNewServer == true)
		{
			if (readLine.find("server") != npos)
			{
				tempClass = new LocationData();
				// serverLength = countServerLength("server", ++whichServer);
				lookForNewServer = false;				
				}
			}
		if (lookForNewServer == false)
		{
			// setData(readLine, "port", 1, *tempClass, whichLine);
			// setData(readLine, "s_name", 2, *tempClass, whichLine);
			// setData(readLine, "error_pages", 3, *tempClass, whichLine);
			// setData(readLine, "client_max_body_size", 4, *tempClass, whichLine);
			// setData(readLine, "location", 5, *tempClass, whichLine);
			serverLength--;
			if (serverLength == 0)
			{
				ContLocationData.push_back(tempClass);
				lookForNewServer = true;
			}
		}
		++whichLine;
	}
}



void ConfigData::setPort(int inputPort) { sPort = inputPort; }

void ConfigData::setServerName(std::string inputServerName) { serverName = inputServerName; }

void ConfigData::setErrorPage(std::string inputErrorPages) { errorPage = inputErrorPages; }

void ConfigData::setBodySize(int inputBodySizes) { sBodySize = inputBodySizes; }



int ConfigData::getPort(void) { return(sPort); }

std::string ConfigData::getServerName(void) { return(serverName); }

std::string ConfigData::getErrorPage(void) { return(errorPage); }

int ConfigData::getBodySize(void) { return(sBodySize); }
