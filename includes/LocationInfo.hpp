#ifndef LocationInfo_HPP
#define LocationInfo_HPP

#include <iostream>
#include <string>
#include <map>

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
	void getLocationInfo();

private:
	std::string _path;
	std::map<std::string, std::string> _info;
};

#endif