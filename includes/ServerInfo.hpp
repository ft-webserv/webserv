#pragma once

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
	void setCgiInfo(std::string cgiInfo);
	void addLocationInfo(LocationInfo *tmp);

	std::map<std::string, std::string> &getServerInfo();
	std::vector<LocationInfo *> &getLocationInfos();
	const std::vector<std::string> &getCgiInfo() const;

	// for test
	void printServerInfo();

private:
	std::map<std::string, std::string> _info;
	std::vector<std::string> _cgiInfo;
	std::vector<LocationInfo *> _locationInfos;
};
