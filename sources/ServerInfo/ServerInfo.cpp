#include "ServerInfo.hpp"

ServerInfo::ServerInfo(void)
{
}

ServerInfo::ServerInfo(std::pair<std::string, std::string> pair)
{
	this->setServerInfo(pair);
}

ServerInfo::ServerInfo(const ServerInfo &src)
{
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
	_serverInfo.insert(pair);
}

void ServerInfo::getServerInfo()
{
	int i = 0;
	std::map<std::string, std::string>::iterator it = _serverInfo.begin();
	for (; it != _serverInfo.end(); it++)
	{
		std::cout << it->first << " : " << it->second << std::endl;
	}
}
