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

	std::cout << "====== \033[1m\033[34mWebServer is opened!!\033[0m ======\n\n";
	for (iter = _conf.getPorts().begin(); iter != _conf.getPorts().end(); iter++)
	{
		if ((servSock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			Exception::socketError();
		int reuse = 1;
		if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
			Exception::socketError();
		_kqueue.setFdset(servSock, SERVER);
		memset(&servAddr, 0, sizeof(servAddr));
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servAddr.sin_port = htons(*iter);
		if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
			Exception::bindError();
		if (listen(servSock, BACKLOG) == -1)
			Exception::listenError();
		fcntl(servSock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		_kqueue.addEvent(servSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		std::cout << "       Socket: \033[32m" << servSock << "\033[0m";
		std::cout << " | Port: \033[32m" << *iter << "\033[0m\n";
	}
	std::cout << "\n===================================\n";
	_monitoringEvent();
}

void ServerManager::_handleErrorFlag(eFdType type, void *udata)
{
	switch (type)
	{
	case SERVER:
		std::cerr << "server socket error!" << std::endl;
		break;
	case CLIENT:
		_disconnectClient(static_cast<Client *>(udata));
		break;
	case CGI:
		_disconnectClient(static_cast<Cgi *>(udata)->getClient());
		break;
	default:
		break;
	}
}

void ServerManager::_handleReadFilter(eFdType type, struct kevent *event)
{
	switch (type)
	{
	case SERVER:
		_acceptClient(event->ident);
		break;
	case CLIENT:
	{
		_handleReadClient(event);
		break;
	}
	case CGI:
	{
		Cgi *cgi = static_cast<Cgi *>(event->udata);

		cgi->readResponse();
		break;
	}
	default:
		break;
	}
}

void ServerManager::_handleWriteFilter(eFdType type, struct kevent *event)
{
	switch (type)
	{
	case CLIENT:
	{
		_handleWriteClient(event);
		break;
	}
	case CGI:
	{
		Cgi *cgi = static_cast<Cgi *>(event->udata);

		cgi->writeBody();
		break;
	}
	default:
		break;
	}
}

void ServerManager::_handleTimerFilter(struct kevent *event)
{
	Client *client = static_cast<Client *>(event->udata);
	if (client->getClientStatus() > START && client->getClientStatus() < FINWRITE)
	{
		std::string errorPagePath = client->getResponse().getErrorPage();

		client->sendErrorPage(event->ident, errorPagePath, _408_REQUEST_TIMEOUT);
	}
	_disconnectClient(client);
}

void ServerManager::_handleReadClient(struct kevent *event)
{
	Client *client = static_cast<Client *>(event->udata);

	if (client->getClientStatus() == START)
		_setRequestTimeOut(client);
	client->readRequest(event);
	if (client->getClientStatus() == READBODY || client->getClientStatus() == READCHUNKEDBODY || client->getClientStatus() == FINREAD)
		_findServerBlock(client);
	if (client->getClientStatus() >= READBODY &&
		client->getRequest().getParsedRequest()._body.size() > client->getResponse().getClientMaxBodySize())
		throw(_413_REQUEST_ENTITY_TOO_LARGE);
	if (client->getClientStatus() == FINREAD)
	{
		_kqueue.disableEvent(event->ident, EVFILT_READ, event->udata);
		_kqueue.enableEvent(event->ident, EVFILT_WRITE, event->udata);
	}
}

void ServerManager::_handleWriteClient(struct kevent *event)
{
	Client *client = static_cast<Client *>(event->udata);

	client->writeResponse();
	if (client->getRequest().getParsedRequest()._connection == "close" && client->getClientStatus() == FINWRITE)
		_disconnectClient(client);
	else if (client->getClientStatus() == FINWRITE)
	{
		_kqueue.enableEvent(event->ident, EVFILT_READ, event->udata);
		_kqueue.disableEvent(event->ident, EVFILT_WRITE, event->udata);
		client->initClient();
		_setKeepAliveTimeOut(client);
	}
}

void ServerManager::_catchKnownError(const eStatus &e, eFdType type, struct kevent *event)
{
	std::string errorPagePath;
	switch (type)
	{
	case CGI:
	{
		Cgi *cgi = static_cast<Cgi *>(event->udata);
		errorPagePath = cgi->getResponse()->getErrorPage();

		cgi->sendErrorPage(cgi->getClientSock(), errorPagePath, e);
		_disconnectClient(cgi->getClient());
		break;
	}
	default:
		Client *client = static_cast<Client *>(event->udata);
		errorPagePath = client->getResponse().getErrorPage();

		client->sendErrorPage(event->ident, errorPagePath, e);
		_disconnectClient(client);
		break;
	}
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
			for (int i = 0; i < numEvents; i++)
			{
				event = &_kqueue.getEventList()[i];
				eFdType type = _kqueue.getFdType(event->ident);
				try
				{
					if (event->flags & EV_ERROR)
						_handleErrorFlag(type, event->udata);
					else if (event->flags & EV_EOF && type == CLIENT)
						_disconnectClient(static_cast<Client *>(event->udata));
					else if (event->filter == EVFILT_READ)
						_handleReadFilter(type, event);
					else if (event->filter == EVFILT_WRITE)
						_handleWriteFilter(type, event);
					else if (event->filter == EVFILT_TIMER)
						_handleTimerFilter(event);
				}
				catch (const eStatus &e)
				{
					_catchKnownError(e, type, event);
				}
				catch (std::exception &e)
				{
					std::cerr << e.what() << std::endl;
					_disconnectClient(static_cast<Client *>(event->udata));
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
		Exception::acceptError();
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

void ServerManager::_disconnectClient(Client *client)
{
	std::cout << "Disconnected with client : " << client->getSocket() << std::endl;
	close(client->getSocket());
	_kqueue._deleteFdType(client->getSocket());
	_kqueue.deleteEvent(client->getSocket(), EVFILT_TIMER, static_cast<void *>(client));
	_kqueue.deleteEvent(client->getSocket(), EVFILT_READ, static_cast<void *>(client));
	_kqueue.deleteEvent(client->getSocket(), EVFILT_WRITE, static_cast<void *>(client));
	if (client->getCgi() != NULL)
	{
		Cgi *cgi = client->getCgi();

		cgi->deleteCgiEvent();
		if (cgi->getPid() != 0 && kill(cgi->getPid(), SIGTERM) == 0)
			waitpid(cgi->getPid(), NULL, 0);
		delete cgi;
	}
	delete client;
}
