#ifndef ConfigData_HPP
# define ConfigData_HPP

# include <iostream>

class ConfigData
{
	private:
		std::string sPort;
		std::string serverName;
		std::string errorPage;
		std::string	sBodySize;

	public:
		ConfigData(void);
		~ConfigData(void);

		void setPorts(std::string inputPort);
		void setServerNames(std::string inputServerName);
		void setErrorPages(std::string inputErrorPages);
		void setBodySizes(std::string inputBodySizes);

		std::string getPort(void);
		std::string getServerName(void);
		std::string getErrorPage(void);
		std::string	getBodySize(void);

};

#endif
