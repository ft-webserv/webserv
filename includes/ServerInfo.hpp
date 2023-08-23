#ifndef ServerInfo_HPP
#define ServerInfo_HPP

#include "LocationInfo.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <string>

class ServerInfo
{
public:
	ServerInfo(void);
	ServerInfo(const ServerInfo &src);
	ServerInfo &operator=(ServerInfo const &rhs);
	virtual ~ServerInfo(void);
	void setServerInfo(std::pair<std::string, std::string> pair);
	void addLocationInfo(LocationInfo *tmp);

	// for test no more
	std::map<std::string, std::string> &getServerInfo();

private:
	std::map<std::string, std::string> _info;
	std::vector<LocationInfo *> _locationInfos;
};

#endif
