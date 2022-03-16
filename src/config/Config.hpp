#ifndef Config_HPP
# define Config_HPP

# include <fstream>
# include <iostream>
# include <sstream>
# include "../../inc/Helper.hpp"
# include <vector>

class Config
{
	private:
		std::string const filename;

		std::string sPort;
		std::string serverName;
		std::string errorPage;
		std::string	sBodySize;

		std::vector<int> vPorts;
		std::vector<std::string> vServerNames;
		std::vector<std::string> vErrorPages;
		std::vector<int> vBodySizes;

	public:
		Config(std::string inArgv1);
		~Config(void);

		void	retrieveValues(void);
		int		errorChecker(void);
		int		countServerLength(int whichServer);
		int		countElement(std::string const & Element);
		void 	setData(std::string readLine, std::string find, int level);
		void	pushContainers(int level);
		//void	pushConfigDataClass(int level);

		std::vector<int> 			getPorts(void);
		std::vector<std::string> 	getServerNames(void);
		std::vector<std::string> 	getErrorPages(void);
		std::vector<int>			getBodySizes(void);

};

#endif
