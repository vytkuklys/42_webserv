#include "Config.hpp"

Config::Config(std::string inArgv1) : filename(inArgv1), npos(-1)
{

	_domain = AF_INET;
	_type = SOCK_STREAM;
	_protocol = 0;
	_interface = INADDR_ANY;
	_backlog = 1024;
	exists_host = false;

	retrieveValues();
}

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

Config::Config(const Config &value) : filename(value.filename), npos(value.npos)
{
	*this = value;
}

Config &Config::operator=(Config const &value)
{
	int size = value.ContConfigData.size();
	int i = 0;
	while (i < size)
	{
		ConfigData *tempClass = new ConfigData(*value.ContConfigData[i]);
		ContConfigData.push_back(tempClass);
		i++;
	}
	this->sPort = value.sPort;
	this->serverName = value.serverName;
	this->errorPage = value.errorPage;
	this->sBodySize = value.sBodySize;
	this->_domain = value._domain;
	this->_type = value._type;
	this->_protocol = value._protocol;
	this->_interface = value._interface;
	this->_backlog = value._backlog;
	this->exists_host = value.exists_host;
	return *this;
}

void Config::pushToClass(int level, ConfigData &tempClass)
{
	if (level == 1)
	{
		if (ft::stoi(sPort) == 0 || ft::stoi(sPort) > 65535 ||
			sPort.length() == 0 || sPort[0] == '-')
		{
			std::cout << "Invalid port: '" << sPort << "'";
			std::cout << ", default port: '8080' used instead" << std::endl;
		}
		else
			tempClass.setPort(ft::stoi(sPort));
		sPort.erase();
	}
	if (level == 2)
	{
		if (serverName.length() == 0 || serverName.length() > 50)
			std::cout << "Invalid srvr_name: '" << serverName << "'" << std::endl;
		else
			tempClass.setServerName(serverName);
		serverName.erase();
	}
	if (level == 3)
	{
		std::ifstream infile(errorPage);
		if (!infile.good() || errorPage.length() == 0 || errorPage.length() > 100)
		{
			std::cout << "Invalid error_pages: '" << errorPage << "'";
			std::cout << ", default error_pages: './documents/html_errors' used instead" << std::endl;
		}
		else
			tempClass.setErrorPage(errorPage);
		errorPage.erase();
	}
	if (level == 4)
	{
		int bytes = 1;
		if (ft::stoi(sBodySize) == 0 || sBodySize.length() == 0 || sBodySize[0] == '-')
		{
			std::cout << "Invalid client_max_body_size: '" << sBodySize << "'";
			std::cout << ", default client_max_body_size: '1B' used instead" << std::endl;
			tempClass.setBodySize(bytes);
		}
		bytes = ft::get_bytes(sBodySize);
		//std::cout << "bytes" <<  bytes << "sBodySize" << sBodySize << std::endl;
		tempClass.setBodySize(bytes);
		sBodySize.erase();
	}
	if (level == 6)
	{
		if (directoryListing != "on" && directoryListing != "off" )
		{
			std::cout << "Invalid directory_listing: '" << directoryListing << "'";
			std::cout << ", default directory_listing: 'on' used instead" << std::endl;
		}
		else
		{
			tempClass.setDirectoryListing(directoryListing);
		}
		directoryListing.erase();
	}
	if (level == 7)
	{
		if (defaultErr.length() == 0 || defaultErr.length() > 100)
		{
			std::cout << "Invalid default_err: '" << defaultErr << "'";
			std::cout << ", default default_err: '404.html' used instead" << std::endl;
		}
		else
			tempClass.setDefaultErr(defaultErr);
		defaultErr.erase();
	}
}

void Config::setData(std::string readLine, std::string find, int level, ConfigData &tempClass, int whichLine)
{
	static int whichLocation = 0;

	size_t begin = readLine.find(find);
	if (begin == npos)
		return;

	if (level == 5)
	{
		if (readLine[readLine.length() - 1] != '{')
		{
			std::cout << "Invalid config file, missing a '{' on line: " << whichLine << std::endl;
			exit(-1);
		}
		tempClass.retrieveValues(filename, whichLine, (whichLine + countServerLength("location", ++whichLocation) - 1));
	}
	else
	{
		if (readLine[readLine.length() - 1] != ';')
		{
			std::cout << "Invalid config file, missing a ';' on line: " << whichLine << std::endl;
			exit(-1);
		}
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
			if (level == 6)
				directoryListing.push_back(readLine[begin + i]);
			if (level == 7)
				defaultErr.push_back(readLine[begin + i]);
			i++;
		}
		pushToClass(level, tempClass);
	}
}

int Config::countServerLength(std::string const &find, int whichOne)
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

int Config::countElement(std::string const &Element)
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

int Config::errorChecker(void)
{
	int i = filename.length();

	if (i < 5)
		return (-1);
	if (filename[i - 5] != '.' && filename[i - 4] != 'c' &&
		filename[i - 3] != 'o' && filename[i - 2] != 'n' &&
		filename[i - 1] != 'f')
		return (-1);
	if (!countElement("server"))
		return (-1);
	if ((countElement("{") != (countElement("server") + countElement("location"))) ||
		(countElement("}") != (countElement("server") + countElement("location"))) ||
		countElement("srvr_name") != countElement("server") ||
		countElement("error_pages") != countElement("server") ||
		countElement("client_max_body_size") != countElement("server") ||
		countElement("directory_listing") != countElement("server") ||
		countElement("port") > countElement("server"))
		return (-1);
	return (0);
}

void Config::retrieveValues(void)
{
	std::string readLine;
	std::ifstream readFile;
	ConfigData *tempClass = nullptr;

	int whichServer = 0;
	int whichLine = 1;
	bool lookForNewServer = true;
	int serverLength = -1;

	readFile.open(filename.c_str());
	if (readFile.is_open())
	{
		if (errorChecker() == -1)
		{
			std::cout << "Invalid config file '" << filename << "'" << std::endl;
			exit(-1);
		}
		while (std::getline(readFile, readLine))
		{
			if (lookForNewServer == true)
			{
				if (readLine.find("server") != npos)
				{
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
				setData(readLine, "directory_listing", 6, *tempClass, whichLine);
				setData(readLine, "default_err", 7, *tempClass, whichLine);
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

std::vector<ConfigData *> &Config::getContConfigData(void) { return (ContConfigData); }

int ft_stoi(std::string s)
{
	int i;
	std::istringstream(s) >> i;
	return (i);
}

std::vector<int> &Config::getPorts(void)
{
	std::vector<ConfigData *>::iterator it = ContConfigData.begin();
	std::vector<ConfigData *>::iterator ite = ContConfigData.end();
	if (ports.size())
		return (ports);
	while (it != ite)
	{
		ports.push_back((*it)->getPort());
		++it;
	}
	return (ports);
}
int Config::getDomain(void) { return (_domain); }
int Config::getType(void) { return (_type); }
int Config::getProtocol(void) { return (_protocol); }
int Config::getBacklog(void) { return (_backlog); }
int Config::getInterface(void) { return (_interface); }

std::string Config::getErrorPage(std::string host)
{
	if (host.empty())
		return ("");
	std::vector<ConfigData *>::iterator it = ContConfigData.begin();
	std::vector<ConfigData *>::iterator ite = ContConfigData.end();

	while (it != ite)
	{
		std::string sPort = ft::to_string((*it)->getPort());
		std::string server_name = (*it)->getServerName() + std::string(":") + sPort;
		bool is_host = is_host_valid(host, server_name, sPort);
		if(is_host)
			return ((*it)->getErrorPage());
		++it;
	}
	return ("");
}

std::string Config::getDefaultErr(std::string host)
{
	if (host.empty())
		return ("");
	std::vector<ConfigData *>::iterator it = ContConfigData.begin();
	std::vector<ConfigData *>::iterator ite = ContConfigData.end();

	while (it != ite)
	{
		std::string sPort = ft::to_string((*it)->getPort());
		std::string server_name = (*it)->getServerName() + std::string(":") + sPort;
		bool is_host = is_host_valid(host, server_name, sPort);
		if(is_host)
			return ((*it)->getDefaultErr());
		++it;
	}
	return ("");
}

bool Config::getDirectoryListing(std::string host)
{
	std::vector<ConfigData *>::iterator it = ContConfigData.begin();
	std::vector<ConfigData *>::iterator ite = ContConfigData.end();

	while (it != ite)
	{
		std::string sPort = ft::to_string((*it)->getPort());
		std::string server_name = (*it)->getServerName() + std::string(":") + sPort;
		bool is_host = is_host_valid(host, server_name, sPort);
		if(is_host)
		{
			std::string dir = (*it)->getDirectoryListing();
			if (dir == "on")
				return (true);
			else
				return (false);
		}
		++it;
	}
	return (false);
}

int Config::get_location_index(std::vector<std::string> locations, std::string paths)
{
	while (!paths.empty() && paths != "/")
	{
		unsigned long pos = paths.find_last_of("/");
		if (pos == std::string::npos)
			return (-1);
		if (static_cast<int>(pos) == 0)
			paths = "/";
		else
			paths = paths.substr(0, pos);
		std::vector<std::string>::iterator it = locations.begin();
		std::vector<std::string>::iterator ite = locations.end();
		int index = 0;
		while (it != ite)
		{
			if (*it == paths)
				return (index);
			++it;
			++index;
		}
	}
	return (-1);
}

LocationData *Config::get_truncated_location(std::vector<std::string> locations, std::string path, std::string host)
{
	int res = get_location_index(locations, path);
	std::vector<ConfigData *> data = getContConfigData();
	std::vector<ConfigData *>::iterator it = data.begin();
	std::vector<ConfigData *>::iterator ite = data.end();
	while (res != -1 && it != ite)
	{
		std::vector<LocationData *> locationData = (*it)->getContLocationData();
		std::vector<LocationData *>::iterator it2 = locationData.begin();
		std::vector<LocationData *>::iterator ite2 = locationData.end();
		std::string sPort = ft::to_string((*it)->getPort());
		std::string server_name = (*it)->getServerName() + std::string(":") + sPort;
		bool is_host = is_host_valid(host, server_name, sPort);
		while (is_host && it2 != ite2)
		{
			if (--res < 0)
				return (*it2);
			++it2;
		}
		++it;
	}
	return (nullptr);
}

LocationData *Config::get_location(std::string host, std::string path)
{
	std::vector<std::string> locations;
	std::vector<ConfigData *> data = getContConfigData();
	std::vector<ConfigData *>::iterator it = data.begin();
	std::vector<ConfigData *>::iterator ite = data.end();
	setHostStatus(false);

	while (it != ite)
	{
		std::string sPort = ft::to_string((*it)->getPort());
		std::string server_name = (*it)->getServerName() + std::string(":") + sPort;
		bool is_host = is_host_valid(host, server_name, sPort);
		if (is_host)
		{
			setHostStatus(true);
		}
		std::vector<LocationData *> locationData = (*it)->getContLocationData();
		std::vector<LocationData *>::iterator it2 = locationData.begin();
		std::vector<LocationData *>::iterator ite2 = locationData.end();
		while (is_host && it2 != ite2)
		{
			std::string location = (*it2)->getLocation();
			if (location != "UNKNOWN")
				locations.push_back(location);
			int i ;
			i = location.find_first_of('*');
			while(static_cast<unsigned long>(i) != std::string::npos)
			{
				location.erase(i, 1);
				i = location.find_first_of('*');
			}
			if (path.find(location) == 0 && (location.size() > 1 || path.size() == 1))
			{
				return (*it2);
			}
			++it2;
		}
		if (is_host)
			break;
		++it;
	}
	LocationData * location = get_truncated_location(locations, path, host);
	return (location);
}

bool Config::getHostStatus(void)
{
	return (exists_host);
}

void Config::setHostStatus(bool status)
{
	exists_host = status;
}

bool is_host_valid(std::string host, std::string server_name, std::string port)
{
	if (host.find_last_of(":") == std::string::npos)
		host.append(":");
	if (host == "localhost:" || host == "127.0.0.1:" || host == "0.0.0.0:") // changed
	{
		std::cout << "if is true" << std::endl;
		return (false);
	}
	if (host.find_last_of(":") + 1 == host.length())
	{
		server_name = server_name.substr(0, server_name.find_last_of(":") + 1);
		const std::string hosts[] = {server_name};
		return (host == server_name);
	}
	const std::string hosts[] = {"localhost" + std::string(":") + port, "127.0.0.1" + std::string(":") + port, "0.0.0.0" + std::string(":") + port, server_name};
	return (ft::is_found(hosts, host, 4));
}
