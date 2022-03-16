#include "ConfigData.hpp"

ConfigData::ConfigData(void) {}

ConfigData::~ConfigData(void) {}


void ConfigData::setPorts(std::string inputPort) { sPort = inputPort; }

void ConfigData::setServerNames(std::string inputServerName) { serverName = inputServerName; }

void ConfigData::setErrorPages(std::string inputErrorPages) { errorPage = inputErrorPages; }

void ConfigData::setBodySizes(std::string inputBodySizes) { sBodySize = inputBodySizes; }


std::string ConfigData::getPort(void) { return(sPort); }

std::string ConfigData::getServerName(void) { return(serverName); }

std::string ConfigData::getErrorPage(void) { return(errorPage); }

std::string ConfigData::getBodySize(void) { return(sBodySize); }
