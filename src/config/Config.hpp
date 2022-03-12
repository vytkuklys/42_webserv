#ifndef Config_HPP
# define Config_HPP

# include <fstream>
# include <iostream>
# include <sstream>
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
		int		countServer(void);
		void 	set_data(std::string readLine, std::string find, int level);

		std::vector<int> 			getPorts(void);
		std::vector<std::string> 	getServerNames(void);
		std::vector<std::string> 	getErrorPages(void);
		std::vector<int>			getBodySizes(void);

};

int ft_stoi(std::string s);

#endif
