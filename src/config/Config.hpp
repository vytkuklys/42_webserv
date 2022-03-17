#ifndef Config_HPP
# define Config_HPP

# include <fstream>
# include <iostream>
# include <sstream>
# include <vector>

# include "../../inc/Helper.hpp"
# include "ConfigData.hpp"

class Config
{
	private:
		std::vector<ConfigData *> ContConfigData;
		// ConfigData * tempClass;

		std::string const filename;
		size_t const npos;

		std::string sPort;
		std::string serverName;
		std::string errorPage;
		std::string	sBodySize;

	public:
		Config(std::string inArgv1);
		~Config(void);

		void	retrieveValues(void);
		int		errorChecker(void);
		int		countServerLength(int whichServer);
		int		countElement(std::string const & Element);
		void 	setData(std::string readLine, std::string find, int level, ConfigData & tempClass);
		void	pushContainers(int level, ConfigData & tempClass);

		std::vector<ConfigData *> & getContConfigData(void);

};

#endif
