#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>

// user define type
typedef unsigned short port_t;

class LocationInfo
{
public:
	LocationInfo(void);
	LocationInfo(const LocationInfo &src);
	LocationInfo &operator=(LocationInfo const &rhs);
	virtual ~LocationInfo(void);
	void setPath(std::string path);
	void setLocationInfo(std::pair<std::string, std::string> pair);

	std::map<std::string, std::string> &getLocationInfo();
	std::string &getPath();

	// for test
	void printLocationInfo();

private:
	std::string _path;
	std::map<std::string, std::string> _info;
};
