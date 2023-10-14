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
			std::cout << "Events num : " << numEvents << std::endl;
			for (int i = 0; i < numEvents; i++)
			{
				event = &_kqueue.getEventList()[i];
				std::cout << "Events : " << event->ident << ", " << event->flags << ", " << event->fflags << ", " << event->filter << ", " << event->data << ", " << event->udata << std::endl;
				eFdType type = _kqueue.getFdType(event->ident);
				try
				{
					std::cout << "----------------------------------" << std::endl;
					std::cout << ((type == SERVER) ? "Server : " : (type == CLIENT) ? "CLIENT : "
																					: "CGI : ")
							  << std::endl;
					std::cout << event->ident << std::endl;
					std::cout << ((event->filter == EVFILT_READ) ? "read" : (event->filter == EVFILT_WRITE) ? "write"
																											: "timmer")
							  << std::endl;
					std::cout << "----------------------------------" << std::endl;
					if (event->flags & EV_ERROR)
					{
						switch (type)
						{
						case SERVER:
							std::cerr << "server socket error!" << std::endl;
							break;
						case CLIENT:
							_disconnectClient(static_cast<Client *>(event->udata));
							break;
						case CGI:
							std::cout << strerror(event->data) << std::endl;
							_disconnectClient(static_cast<Cgi *>(event->udata)->getClient());
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
						{
							Client *client = static_cast<Client *>(event->udata);

							if (client->getClientStatus() == START)
								_setRequestTimeOut(client);
							client->readRequest(event);
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
						}
						case CGI:
						{
							Cgi *cgi = static_cast<Cgi *>(event->udata);

							cgi->readResponse();
							if (cgi->getIsCgiFin() == true)
								cgi->getClient()->setCgi(NULL);
							break;
						}
						default:
							break;
						}
					}
					else if (event->filter == EVFILT_WRITE)
					{
						switch (type)
						{
						case CLIENT:
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
					else if (event->filter == EVFILT_TIMER)
					{
						Client *client = static_cast<Client *>(event->udata);

						if (client->getClientStatus() > START && client->getClientStatus() < FINWRITE)
						{
							std::string errorPagePath = client->getResponse().getErrorPage();

							client->sendErrorPage(event->ident, errorPagePath, _408_REQUEST_TIMEOUT);
						}
						_disconnectClient(client);
					}
				}
				catch (const eStatus &e)
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

void ServerManager::_disconnectClient(Client *client)
{
	std::cout << "Disconnected with client : " << client->getSocket() << std::endl;
	_kqueue.deleteEvent(client->getSocket(), EVFILT_TIMER, static_cast<void *>(client));
	_kqueue.deleteEvent(client->getSocket(), EVFILT_READ, static_cast<void *>(client));
	_kqueue.deleteEvent(client->getSocket(), EVFILT_WRITE, static_cast<void *>(client));
	_kqueue._deleteFdType(client->getSocket());
	close(client->getSocket());
	if (client->getCgi() != NULL)
	{
		Cgi *cgi = client->getCgi();

		if (kill(cgi->getPid(), SIGTERM))
			waitpid(cgi->getPid(), NULL, 0);
		cgi->deleteCgiEvent();
		delete cgi;
	}
	delete client;
}

// void ServerManager::_disconnectCGI(struct kevent *event)
// {
// 	size_t size;
// 	Cgi *cgi = static_cast<Cgi *>(event->udata);

// 	if (kill(cgi->getPid(), SIGTERM))
// 		waitpid(cgi->getPid(), NULL, 0);
// 	cgi->deleteCgiEvent();
// 	size = _kqueue.getEventList().size();
// 	for (size_t i = 0; i < size; i++)
// 	{
// 		if (_kqueue.getEventList()[i].ident == cgi->getClientSock())
// 		{
// 			_disconnectClient(&_kqueue.getEventList()[i]);
// 			break;
// 		}
// 	}
// 	if (cgi != NULL)
// 		delete cgi;
// }
