#include "LocationData.hpp"

LocationData::LocationData(void) : root("UNKNOWN"), 
method("UNKNOWN"), index("UNKNOWN") {}

LocationData::~LocationData(void) {}



void LocationData::setRoot(std::string inputRoot) { root = inputRoot; }

void LocationData::setMethod(std::string inputMethod) { method = inputMethod; }

void LocationData::setIndex(std::string inputIndex) { index = inputIndex; }



std::string LocationData::getRoot(void) { return(root); }

std::string LocationData::getMethod(void) { return(method); }

std::string LocationData::getIndex(void) { return(index); }
