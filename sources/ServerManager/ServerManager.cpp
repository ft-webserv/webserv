#include "ServerManager.hpp"

ServerManager::ServerManager()
	: _kqueue(Kqueue::getInstance())
{
}

ServerManager::ServerManager(const ServerManager &src)
	: _kqueue(Kqueue::getInstance())
{
	(void)src;
}

ServerManager &ServerManager::operator=(ServerManager const &rhs)
{
	(void)rhs;
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
		int reuse = 1;
		if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
			Exception::socketError("setsockopt() error!");
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
				try
				{
					// std::cout << ((type == SERVER) ? "Server : " : "Client : ");
					// std::cout << event->ident << std::endl;
					// std::cout << ((event->filter == EVFILT_READ) ? "read" : (event->filter == EVFILT_WRITE) ? "write"
					// 																						: "timmer")
					// 		  << std::endl;
					if (event->flags & EV_ERROR)
					{
						switch (type)
						{
						case SERVER:
							std::cerr << "server socket error!" << std::endl;
							break;
						case CLIENT:
							_disconnectClient(event);
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
							if (client->getClientStatus() == START)
								_setRequestTimeOut(client);
							client->readRequest();
							if (client->getClientStatus() == READBODY || client->getClientStatus() == READCHUNKEDBODY || client->getClientStatus() == FINREAD)
							{
								_findServerBlock(client);
							}
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
						switch (type)
						{
						case CLIENT:
							Client *client = static_cast<Client *>(event->udata);
							client->writeResponse();
							if (client->getRequest().getParsedRequest()._connection == "close" && client->getClientStatus() == FINWRITE)
								_disconnectClient(event);
							else if (client->getClientStatus() == FINWRITE)
							{
								_kqueue.enableEvent(event->ident, EVFILT_READ, event->udata);
								_kqueue.disableEvent(event->ident, EVFILT_WRITE, event->udata);
								client->initClient();
								_setKeepAliveTimeOut(client);
							}
							break;
						case CGI:
							Cgi *cgi = static_cast<Cgi *>(event->udata);
							break;
						default:
							break;
						}
					}
					else if (event->filter == EVFILT_TIMER)
					{
						Client *client = static_cast<Client *>(event->udata);

						if (client->getClientStatus() > START && client->getClientStatus() < FINWRITE)
						{
							std::string errorPagePath = client->getResponse().getErrorPage();

							client->sendErrorPage(event->ident, errorPagePath, _408_REQUEST_TIMEOUT);
						}
						_disconnectClient(event);
					}
				}
				catch (const eStatus &e)
				{
					Client *client = static_cast<Client *>(event->udata);
					std::string errorPagePath = client->getResponse().getErrorPage();

					client->sendErrorPage(event->ident, errorPagePath, e);
					_disconnectClient(event);
				}
				catch (std::exception &e)
				{
					std::cerr << e.what() << std::endl;
					_disconnectClient(event);
				}
			}
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
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
	_setKeepAliveTimeOut(client);
}

void ServerManager::_setKeepAliveTimeOut(Client *client)
{
	Config &conf = Config::getInstance();

	_kqueue.addEvent(client->getSocket(), EVFILT_TIMER,
					 EV_ADD | EV_ONESHOT, NOTE_SECONDS,
					 conf.getKeepAliveTime(), static_cast<void *>(client));
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
	std::cout << "Disconnected with client : " << client->getSocket() << std::endl;
	_kqueue.addEvent(client->getSocket(), EVFILT_TIMER, EV_DELETE, 0, 0, static_cast<void *>(client));
	_kqueue.addEvent(client->getSocket(), EVFILT_READ, EV_DELETE, 0, 0, static_cast<void *>(client));
	_kqueue.addEvent(client->getSocket(), EVFILT_WRITE, EV_DELETE, 0, 0, static_cast<void *>(client));
	_kqueue._deleteFdType(event->ident);
	close(client->getSocket());
	if (client != NULL)
	{
		delete client;
		client = NULL;
	}
}
