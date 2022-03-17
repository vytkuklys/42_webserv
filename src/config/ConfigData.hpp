#ifndef ConfigData_HPP
# define ConfigData_HPP

# include <fstream>
# include <iostream>
# include <sstream>
# include <vector>

# include "LocationData.hpp"

class ConfigData
{
	private:
		std::vector<LocationData *> ContLocationData;
		
		size_t const npos;

		int sPort;
		std::string serverName;
		std::string errorPage;
		int	sBodySize;

		std::string tempLocation;		
		std::string tempRoot;
		std::string tempMethod;
		std::string tempIndex;

	public:
		ConfigData(void);
		~ConfigData(void);

		void retrieveValues(std::string const filename);

		void setPort(int inputPort);
		void setServerName(std::string inputServerName);
		void setErrorPage(std::string inputErrorPages);
		void setBodySize(int inputBodySizes);

		int getPort(void);
		std::string getServerName(void);
		std::string getErrorPage(void);
		int	getBodySize(void);

};

#endif
