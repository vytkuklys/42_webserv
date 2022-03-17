#ifndef ConfigData_HPP
# define ConfigData_HPP

# include <iostream>

class ConfigData
{
	private:
		int sPort;
		std::string serverName;
		std::string errorPage;
		int	sBodySize;

	public:
		ConfigData(void);
		~ConfigData(void);

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
