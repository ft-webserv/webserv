#include "ServerInfo.hpp"

ServerInfo::ServerInfo(void)
{
}

ServerInfo::ServerInfo(const ServerInfo &src)
{
	*this = src;
}

ServerInfo::~ServerInfo(void)
{
}

ServerInfo &ServerInfo::operator=(ServerInfo const &rhs)
{
	if (this != &rhs)
	{
		this->_info = rhs._info;
		this->_locationInfos = rhs._locationInfos;
	}
	return *this;
}

void ServerInfo::setServerInfo(std::pair<std::string, std::string> pair)
{
	_info.insert(pair);
}

void ServerInfo::addLocationInfo(LocationInfo *tmp)
{
	_locationInfos.push_back(tmp);
}

std::map<std::string, std::string> &ServerInfo::getServerInfo()
{
	return (_info);
}

std::vector<LocationInfo *> &ServerInfo::getLocationInfos()
{
	return (_locationInfos);
}

void ServerInfo::printServerInfo()
{
	std::map<std::string, std::string>::iterator it = _info.begin();
	for (; it != _info.end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;
}
