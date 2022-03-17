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
		int		countServerLength(std::string const & find, int whichOne);
		int		countElement(std::string const & Element);
		void 	setData(std::string readLine, std::string find, int level, ConfigData & tempClass, int whichLine);
		void	pushToClass(int level, ConfigData & tempClass);

		std::vector<ConfigData *> & getContConfigData(void);

};

#endif
