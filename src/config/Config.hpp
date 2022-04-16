#pragma once

#ifndef Config_HPP
#define Config_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stddef.h>

#include "ConfigData.hpp"
#include "../../inc/Helper.hpp"

class ConfigData;

class Config
{
private:
	std::vector<ConfigData *> ContConfigData;
	std::string const filename;
	size_t const npos;

	std::vector<int> ports;

	std::string sPort;
	std::string serverName;
	std::string errorPage;
	std::string defaultErr;
	std::string sBodySize;
	std::string directoryListing;

	int _domain;
	int _type;
	int _protocol;
	int _interface;
	int _backlog;

	bool exists_host;

	int get_location_index(std::vector<std::string> locations, std::string paths);
	LocationData * get_truncated_location(std::vector<std::string> locations, std::string path, std::string host);

public:
	Config(std::string inArgv1);
	~Config(void);
	Config(const Config &value);
	Config &operator=(Config const &value);

	void retrieveValues(void);
	int errorChecker(void);
	int countServerLength(std::string const &find, int whichOne);
	int countElement(std::string const &Element);
	void setData(std::string readLine, std::string find, int level, ConfigData &tempClass, int whichLine);
	void pushToClass(int level, ConfigData &tempClass);
	void setHostStatus(bool status);

	std::vector<int> &getPorts(void);
	std::vector<ConfigData *> &getContConfigData(void);
	int getType(void);
	int getDomain(void);
	int getBacklog(void);
	int getProtocol(void);
	int getInterface(void);
	bool getHostStatus(void);
	std::string getErrorPage(std::string host);
	std::string getDefaultErr(std::string host);
	bool getDirectoryListing(std::string port);
	LocationData * get_location(std::string host, std::string path);
};
bool is_host_valid(std::string host, std::string server_name, std::string port);

#endif
