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
	struct sockaddr_in servAddr;
	int servSock;

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
		_servers.insert(std::pair<port_t, Server *>(*iter, _addServer(*iter)));
	}
	_monitoringEvent();
}

Server *ServerManager::_addServer(port_t port)
{
	Server *val;
	std::map<std::string, std::string>::iterator it;

	val = new Server();
	for (int i = 0; i < _conf.getServerInfos().size(); i++)
	{
		for (it = _conf.getServerInfos()[i]->getServerInfo().begin(); it != _conf.getServerInfos()[i]->getServerInfo().end(); it++)
		{
			if (it->second == "listen")
			{
				if (atoi((it->first).c_str()) == port)
				{
					if (val->getDefaultServer() == NULL)
						val->setDefaultServer(_conf.getServerInfos()[i]);
					val->setHostServer(_conf.getServerInfos()[i]);
				}
			}
		}
	}
	return (val);
}

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
					_readRequest(event->ident, event->data);
					break;
				default:
					break;
				}
			}
			// else if (event->flags & EVFILT_WRITE)
			// 	_writeResponse(event->ident);
		}
	}
}

void ServerManager::_acceptClient(uintptr_t &servSock)
{
	uintptr_t clntSock;
	struct sockaddr_in serv;

	if ((clntSock = accept(servSock, NULL, NULL)) == -1)
		Exception::acceptError("accept() error!");
	fcntl(clntSock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	_kqueue.addEvent(clntSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	_kqueue.addEvent(clntSock, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
	_kqueue.setFdset(clntSock, CLIENT);
	Request *tmp = new Request();
	_requests.insert(std::pair<uintptr_t, Request *>(clntSock, tmp));
}

void ServerManager::_readRequest(uintptr_t &clntSock, intptr_t data)
{
	ssize_t len;
	char buf[BUFFERSIZE + 1];

	if ((len = recv(clntSock, buf, BUFFERSIZE, 0)) == -1)
		Exception::recvError("recv() error!");
	else if (len <= 0)
		Exception::disconnectDuringRecvError("diconnected during read!");
	_requests.find(clntSock)->second->setRequestBufs(buf);
	memset(buf, 0, BUFFERSIZE + 1);
	if (data <= BUFFERSIZE)
		_requests.find(clntSock)->second->parseRequest();
}

void ServerManager::_writeResponse(uintptr_t clntSock)
{
	struct sockaddr_in clnt;
	socklen_t clntSockLen = sizeof(clnt);
	port_t port;

	getsockname(clntSock, (sockaddr *)&clnt, &clntSockLen);
	port = ntohs(clnt.sin_port);
	_servers.find(port)->second->makeResponse(_requests.find(clntSock)->second, clntSock);
}
