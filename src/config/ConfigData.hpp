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

		std::string getPorts(void);
		std::string getServerNames(void);
		std::string getErrorPages(void);
		std::string	getBodySizes(void);

};

#endif
