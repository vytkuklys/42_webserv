#ifndef Config_HPP
#define Config_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "../../inc/Helper.hpp"
#include "ConfigData.hpp"

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
	LocationData * get_truncated_location(std::vector<std::string> locations, std::string path, std::string port);

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

	std::vector<int> &getPorts(void);
	std::vector<ConfigData *> &getContConfigData(void);
	int getType(void);
	int getDomain(void);
	int getBacklog(void);
	int getProtocol(void);
	int getInterface(void);
	bool getHostStatus(void);
	std::string getErrorPage(std::string port);
	std::string getDefaultErr(std::string port);
	bool getDirectoryListing(std::string port);
	LocationData * get_location(std::string port, std::string path);
	std::string get_hostname(std::string port);
};
bool is_host_valid(std::string host, std::string server_name);

#endif
