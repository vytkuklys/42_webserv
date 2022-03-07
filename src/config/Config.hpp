#ifndef Config_HPP
# define Config_HPP

# include <iostream>

class Config
{
	private:
		std::string argv1;

		std::string serverName;
		std::string errorPage;
		int port;
		int	bodySize;

	public:
		Config(std::string inArgv1);
		~Config(void);

		std::string const getServerName(void);
		std::string const getErrorPage(void);
		int getPort(void);
		int	getBodySize(void);

};

#endif
