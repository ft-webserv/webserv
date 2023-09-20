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
		try
		{
			numEvents = _kqueue.doKevent();
			// std::cout << "Events num : " << numEvents << std::endl;
			for (int i = 0; i < numEvents; i++)
			{
				event = &_kqueue.getEventList()[i];
				eFdType type = _kqueue.getFdType(event->ident);
				// std::cout << ((type == SERVER) ? "Server : " : "Client : ");
				// std::cout << ((event->filter == EVFILT_READ) ? "read" : "write") << std::endl;
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
						Client *client;
						client = static_cast<Client *>(event->udata);
						_setRequestTimeOut(client);
						client->readRequest();
						if (client->getClientStatus() == FINREAD)
						{
							_kqueue.disableEvent(event->ident, EVFILT_READ, event->udata);
							_kqueue.enableEvent(event->ident, EVFILT_WRITE, event->udata);
						}
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
					if (client->getRequest().getParsedRequest()._contentType == "close")
						_disconnectClient(event);
					else
					{
						_kqueue.enableEvent(event->ident, EVFILT_READ, event->udata);
						_kqueue.disableEvent(event->ident, EVFILT_WRITE, event->udata);
					}
				}
				else if (event->filter == EVFILT_TIMER)
				{
					Client *client = static_cast<Client *>(event->udata);
					if (client->getClientStatus() < FINREAD)
						// Error page;
						_disconnectClient(event);
				}
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << '\n';
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
	client->setKeepAliveTime();
	_setKeepAliveTimeOut(client);
}

void ServerManager::_setKeepAliveTimeOut(Client *client)
{
	_kqueue.addEvent(client->getSocket(), EVFILT_TIMER,
					 EV_ADD | EV_ONESHOT, NOTE_SECONDS,
					 client->getKeepAliveTime(), static_cast<void *>(client));
}

void ServerManager::_setRequestTimeOut(Client *client)
{
	Config &conf = Config::getInstance();

	_kqueue.addEvent(client->getSocket(), EVFILT_TIMER,
					 EV_ADD | EV_ONESHOT, NOTE_SECONDS,
					 conf.getRequestTime(), static_cast<void *>(client));
}

port_t ServerManager::_findOutPort(uintptr_t clntsock)
{
	struct sockaddr_in clnt;
	socklen_t clntSockLen = sizeof(clnt);
	port_t port;

	getsockname(clntsock, (sockaddr *)&clnt, &clntSockLen);
	port = ntohs(clnt.sin_port);
	return (port);
}

void ServerManager::_findServerBlock(Client *client)
{
	client->setServerBlock(_findOutPort(client->getSocket()));
}

void ServerManager::_disconnectClient(struct kevent *event)
{
	Client *client = static_cast<Client *>(event->udata);
	_kqueue._deleteFdType(event->ident);
	close(client->getSocket());
	delete client;
}
