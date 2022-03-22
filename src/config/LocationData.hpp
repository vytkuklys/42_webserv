#ifndef LocationData_HPP
#define LocationData_HPP

#include <iostream>

class LocationData
{
private:
	std::string location;
	std::string root;
	std::string method;
	std::string index;

public:
	LocationData(void);
	~LocationData(void);
	LocationData(const LocationData &value);
	LocationData &operator=(LocationData const &value);

	void setLocation(std::string inLocation);
	void setRoot(std::string inputRoot);
	void setMethod(std::string inputMethod);
	void setIndex(std::string inputIndex);

	std::string getLocation(void);
	std::string getRoot(void);
	std::string getMethod(void);
	std::string getIndex(void);
};

#endif
