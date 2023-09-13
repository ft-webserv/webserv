#include "ServerInfo.hpp"

ServerInfo::ServerInfo(void)
{
}

ServerInfo::ServerInfo(const ServerInfo &src)
{
	this->_info = src._info;
	this->_locationInfos = src._locationInfos;
}

ServerInfo::~ServerInfo(void)
{
}

ServerInfo &ServerInfo::operator=(ServerInfo const &rhs)
{
	if (this != &rhs)
	{
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
	// std::map<std::string, std::string>::iterator it = _info.begin();
	// for (; it != _info.end(); it++)
	// {
	// 	std::cout << it->first << " : " << it->second << std::endl;
	// }
	// for (int i = 0; i < _locationInfos.size(); i++)
	// {
	// 	_locationInfos[i]->getLocationInfo();
	// }
}

std::vector<LocationInfo *> ServerInfo::getLocationInfo()
{
	return (_locationInfos);
}
