#include "ServerManager.hpp"

ServerManager::ServerManager()
{
}

ServerManager::ServerManager(const ServerManager &src)
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
	Config &_conf = Config::getInstance();

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
			else if (event->filter == EVFILT_READ)
			{
				switch (type)
				{
				case SERVER:
					_acceptClient(event->ident);
					break;
				case CLIENT:
					static_cast<Client *>(event->udata)->readRequest(event->data); // 추후 참조자에 담아서 사용할 예정
					_kqueue.disableEvent(event->ident, EVFILT_READ, event->udata);
					_kqueue.enableEvent(event->ident, EVFILT_WRITE, event->udata); // readRequest 안에서 읽은 길이가 content-length를 만족하면, read event disable & write evnet enable
					break;
				default:
					break;
				}
			}
			else if (event->filter == EVFILT_WRITE)
			{
				Client *client = static_cast<Client *>(event->udata);
				_findServerBlock(client);
				client->writeResponse();
			}
		}
	}
}

void ServerManager::_acceptClient(uintptr_t &servSock)
{
	uintptr_t clntSock;
	Client *client;

	if ((clntSock = accept(servSock, NULL, NULL)) == -1)
		Exception::acceptError("accept() error!");
	fcntl(clntSock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	client = new Client(clntSock);
	_kqueue.addEvent(clntSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, static_cast<void *>(client));
	_kqueue.addEvent(clntSock, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, static_cast<void *>(client));
	_kqueue.setFdset(clntSock, CLIENT);
	int res = _kqueue.doKevent();
	if (res == -1)
		Exception::keventError("kevent() error Event!");
}

void ServerManager::_findServerBlock(Client *client)
{
	struct sockaddr_in clnt;
	socklen_t clntSockLen = sizeof(clnt);
	port_t port;

	getsockname(client->getSocket(), (sockaddr *)&clnt, &clntSockLen);
	port = ntohs(clnt.sin_port);
	client->setServerBlock(port);
}
