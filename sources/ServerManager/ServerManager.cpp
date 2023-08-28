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

int ServerManager::newServSock()
{
	std::set<int>::iterator iter;
	int servSock;
	struct sockaddr_in servAddr;

	for (iter = _conf.getPorts().begin(); iter != _conf.getPorts().end(); iter++)
	{
		if (servSock = socket(PF_INET, SOCK_STREAM, 0) == -1)
		{
			Exception::socketError("socket() error!");
		}
		memset(&servAddr, 0, sizeof(servAddr));
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servAddr.sin_port = htons(*iter);
		if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
		{
			Exception::bindError("bind() error!");
		}
		if (listen(servSock, BACKLOG) == -1)
		{
			Exception::listenError("listen() error!");
		}
		fcntl(servSock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		_kqueue.addEvent(servSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		_monitoringEvent();
	}
}

void ServerManager::_addServer()
{
	Server *val;
	std::string key;
	std::map<std::string, std::string>::iterator it;

	for (int i = 0; i < _conf.getServerInfos().size(); i++)
	{
		val = new Server();
		for (it = _conf.getServerInfos()[i]->getServerInfo().begin(); it != _conf.getServerInfos()[i]->getServerInfo().end(); it++)
		{
			if (it->second == "server_name")
			{
			}
		}
	}
}

Server *ServerManager::_setServer(void)
{
	Server *tmp;
}

void ServerManager::_monitoringEvent()
{
	int numEvents;

	numEvents = kevent();
	while (true)
	{
	}
}
