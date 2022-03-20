#ifndef Config_HPP
# define Config_HPP

# include <fstream>
# include <iostream>
# include <sstream>
# include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

# include "../../inc/Helper.hpp"
# include "ConfigData.hpp"

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
		std::string	sBodySize;
		int _domain;
		int _type;
		int	_protocol;
		int _interface;
		int _backlog;

	public:
		Config(std::string inArgv1);
		~Config(void);

		void	retrieveValues(void);
		int		errorChecker(void);
		int		countServerLength(std::string const & find, int whichOne);
		int		countElement(std::string const & Element);
		void 	setData(std::string readLine, std::string find, int level, ConfigData & tempClass, int whichLine);
		void	pushToClass(int level, ConfigData & tempClass);

		std::vector<int>& getPorts(void);
		std::vector<ConfigData *> & getContConfigData(void);
		int getDomain(void);
		int getType(void);
		int getProtocol(void);
		int getBacklog(void);
		int getInterface(void);
};

#endif
