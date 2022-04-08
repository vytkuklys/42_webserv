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
	std::string script;
	int max_body;

public:
	LocationData(void);
	~LocationData(void);
	LocationData(const LocationData &value);
	LocationData &operator=(LocationData const &value);

	void setLocation(std::string inLocation);
	void setRoot(std::string inputRoot);
	void setMethod(std::string inputMethod);
	void setIndex(std::string inputIndex);
	void setScript(std::string inputScript);
	void setMaxBody(int value);

	std::string getLocation(void);
	std::string getRoot(void);
	std::string getMethod(void);
	std::string getIndex(void);
	std::string getScript(void);
	int getMaxBody(void);
};

#endif
