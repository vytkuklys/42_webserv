#include "ConfigData.hpp"

ConfigData::ConfigData(void) : sPort(0), serverName("UNKNOWN"), 
errorPage("UNKNOWN"), sBodySize(0) {}

ConfigData::~ConfigData(void) { /*!!DELETE Location classes !!*/ }


void ConfigData::setPort(int inputPort) { sPort = inputPort; }

void ConfigData::setServerName(std::string inputServerName) { serverName = inputServerName; }

void ConfigData::setErrorPage(std::string inputErrorPages) { errorPage = inputErrorPages; }

void ConfigData::setBodySize(int inputBodySizes) { sBodySize = inputBodySizes; }


int ConfigData::getPort(void) { return(sPort); }

std::string ConfigData::getServerName(void) { return(serverName); }

std::string ConfigData::getErrorPage(void) { return(errorPage); }

int ConfigData::getBodySize(void) { return(sBodySize); }
