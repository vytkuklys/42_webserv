#include "Config.hpp"
#include <sstream>

Config::Config(std::string inArgv1) : filename(inArgv1) {}

Config::~Config(void) {}

void Config::set_data(std::string readLine, std::string find, int level)
{
	int begin = readLine.find(find);
	if (begin == -1)
		return ;
	
	begin = begin + find.length();
	int i = 0;

	while (isspace(readLine[begin + i]))
		i++;
	
	while (readLine[begin + i] != ';')
	{
		if (level == 1)
			sPort.push_back(readLine[begin + i]);
		if (level == 2)
			serverName.push_back(readLine[begin + i]);
		if (level == 3)
			errorPage.push_back(readLine[begin + i]);
		if (level == 4)
			sBodySize.push_back(readLine[begin + i]);
		i++;
	}
}

void	Config::retrieveValues(void)
{
	std::string readLine;
	std::ifstream readFile;

	readFile.open(filename.c_str());
	if (readFile.is_open())
	{
		while (getline(readFile, readLine))
		{
			set_data(readLine, "listen", 1);
			// set_data(readLine, "server_name", 2);
			set_data(readLine, "error_page", 3);
			set_data(readLine, "client_max_body_size", 4);
			// readLine = ft_replace(readLine, argv[2], argv[3]);
			// if (readFile.peek() != EOF)
		}
	}
	else
		std::cout << "Unable to open file: " << filename << std::endl;
}

// std::stoi is C11 so we have to write our on
static int stoi( std::string s ) {
    int i;
    std::istringstream(s) >> i;
    return i;
}

int Config::getPort(void) { return(stoi(sPort)); }

// std::string const Config::getServerName(void) { return(serverName); }

std::string const Config::getErrorPage(void) { return(errorPage); }

int	Config::getBodySize(void) { return(stoi(sBodySize)); }
