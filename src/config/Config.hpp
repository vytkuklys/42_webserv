#ifndef Config_HPP
# define Config_HPP

# include <fstream>
# include <iostream>
# include <stdlib.h>
# include <string>

# include <iostream>
# include <stdio.h>
# include <ctype.h>
# include <stdlib.h> 

class Config
{
	private:
		std::string const filename;

		std::string serverName;
		std::string errorPage;
		std::string sPort;
		int	bodySize;

	public:
		Config(std::string inArgv1);
		~Config(void);

		void	retrieveValues(void);
		void 	set_data(std::string readLine, std::string find);

		std::string const getServerName(void);
		std::string const getErrorPage(void);
		int getPort(void);
		int	getBodySize(void);

};

#endif
