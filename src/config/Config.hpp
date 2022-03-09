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

		std::string sPort;
		std::string serverName;
		std::string errorPage;
		std::string	sBodySize;

	public:
		Config(std::string inArgv1);
		~Config(void);

		void	retrieveValues(void);
		void 	set_data(std::string readLine, std::string find, int level);

		int getPort(void);
		// std::string const getServerName(void);
		std::string const getErrorPage(void);
		int	getBodySize(void);

};

#endif
