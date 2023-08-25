#include "ServerManager.hpp"

ServerManager::ServerManager(Config &conf)
	: _conf(conf)
{
}

ServerManager::ServerManager(const ServerManager &src)
	: _conf(src._conf)
{
}

ServerManager &ServerManager::operator=(ServerManager const &rhs)
{
}

ServerManager::~ServerManager(void)
{
}

void ServerManager::openServSock(void)
{

}

void ServerManager::addServer(void)
{
	Server *val;
	std::string key;
	std::map<std::string, std::string>::iterator it;

	for (int i = 0; i < _conf.getServerInfos().size(); i++)
	{
		val = new Server();
		for (it = _conf.getServerInfos()[i]->getServerInfo().begin(); it != _conf.getServerInfos()[i]->getServerInfo().end(); it++)
		{
			if (it->second == "listen")
			{
			}
		}
	}
}

Server *ServerManager::setServer(void)
{
	Server *tmp;
}
