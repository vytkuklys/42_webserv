#include "LocationData.hpp"

LocationData::LocationData() : location("UNKNOWN"), root("UNKNOWN"),
method("UNKNOWN"), index("UNKNOWN"), script("UNKNOWN")/*, max_body(1)*/ {}

LocationData::~LocationData(void) {}

LocationData::LocationData(const LocationData &value)
{
    *this = value;
}

LocationData &LocationData::operator=(LocationData const &value)
{
    this->location = value.location;
    this->root = value.root;
    this->method = value.method;
    this->index = value.index;
	this->script = value.script;
    return *this;
}


void LocationData::setLocation(std::string inLocation) { location = inLocation; }

void LocationData::setRoot(std::string inputRoot) { root = inputRoot; }

void LocationData::setMethod(std::string inputMethod) { method = inputMethod; }

void LocationData::setIndex(std::string inputIndex) { index = inputIndex; }

void LocationData::setScript(std::string inputScript) { script = inputScript; }

void LocationData::setMaxBody(int value) { max_body = value; }

std::string LocationData::getLocation(void) { return(location); }

std::string LocationData::getRoot(void) { return(root); }

std::string LocationData::getMethod(void) { return(method); }

std::string LocationData::getIndex(void) { return(index); }

std::string LocationData::getScript(void) { return(script); }

int LocationData::getMaxBody(void) { return(max_body); }
