#include "ConfigData.hpp"

ConfigData::ConfigData(void) {}

ConfigData::~ConfigData(void) {}


void ConfigData::setPorts(std::string inputPort) { sPort = inputPort; }

void ConfigData::setServerNames(std::string inputServerName) { serverName = inputServerName; }

void ConfigData::setErrorPages(std::string inputErrorPages) { errorPage = inputErrorPages; }

void ConfigData::setBodySizes(std::string inputBodySizes) { sBodySize = inputBodySizes; }


std::string ConfigData::getPorts(void) { return(sPort); }

std::string ConfigData::getServerNames(void) { return(serverName); }

std::string ConfigData::getErrorPages(void) { return(errorPage); }

std::string ConfigData::getBodySizes(void) { return(sBodySize); }
