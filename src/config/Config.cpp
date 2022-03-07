#include "Config.hpp"

Config::Config(std::string inArgv1) : argv1(inArgv1) {}

Config::~Config(void) {}

std::string const Config::getServerName(void) { return(serverName); }

std::string const Config::getErrorPage(void) { return(errorPage); }

int Config::getPort(void) { return(port); }

int	Config::getBodySize(void) { return(bodySize); }
