#pragma once

#include <iostream>
#include <string>
#include <map>

// user define type
typedef unsigned short port_t;

class LocationInfo
{
public:
	LocationInfo(void);
	LocationInfo(const LocationInfo &src);
	LocationInfo &operator=(LocationInfo const &rhs);
	virtual ~LocationInfo(void);
	void setPath(std::string);
	void setLocationInfo(std::pair<std::string, std::string> pair);

	// for test
	const std::map<std::string, std::string> getLocationInfo() const;
	const std::string getPath() const;

private:
	std::string _path;
	std::map<std::string, std::string> _info;
};
