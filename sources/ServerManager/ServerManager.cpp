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
	return (*this);
}

ServerManager::~ServerManager(void)
{
}

void ServerManager::runServer()
{
	std::set<int>::iterator iter;
	int servSock;
	struct sockaddr_in servAddr;

	for (iter = _conf.getPorts().begin(); iter != _conf.getPorts().end(); iter++)
	{
		if ((servSock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			Exception::socketError("socket() error!");
		_kqueue.setFdset(servSock, SERVER);
		memset(&servAddr, 0, sizeof(servAddr));
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servAddr.sin_port = htons(*iter);
		if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
			Exception::bindError("bind() error!");
		if (listen(servSock, BACKLOG) == -1)
			Exception::listenError("listen() error!");
		fcntl(servSock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		_kqueue.addEvent(servSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	}
	_monitoringEvent();
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

// Server *ServerManager::_setServer(void)
// {
// 	Server *tmp;
// }

void ServerManager::_monitoringEvent()
{
	int numEvents;
	struct kevent *event;

	while (true)
	{
		numEvents = _kqueue.doKevent();
		for (int i = 0; i < numEvents; i++)
		{
			event = &_kqueue.getEventList()[i];
			eFdType type = _kqueue.getFdType(event->ident);
			if (event->flags & EV_ERROR)
			{
				switch (type)
				{
				case SERVER:
					std::cerr << "server socket error!" << std::endl;
					break;
				case CLIENT:
					_kqueue.deleteEvent(event->ident);
					break;
				default:
					break;
				}
			}
			else if (event->flags & EVFILT_READ)
			{
				switch (type)
				{
				case SERVER:
					_acceptClient(event->ident);
					break;
				case CLIENT:
					std::cout << "Hi im client!" << std::endl;
					break;
				default:
					break;
				}
			}
			else if (event->flags & EVFILT_WRITE)
			{
			}
		}
	}
}

void ServerManager::_acceptClient(uintptr_t servSock)
{
	uintptr_t clntSock;

	if ((clntSock = accept(servSock, NULL, NULL)) == -1)
		Exception::acceptError("accept() error!");
	fcntl(clntSock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	_kqueue.addEvent(clntSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	_kqueue.addEvent(clntSock, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	_kqueue.setFdset(clntSock, CLIENT);
}
