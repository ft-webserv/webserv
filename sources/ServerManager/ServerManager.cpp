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
		_servers.insert(std::pair<uintptr_t, Server *>(servSock, _addServer(*iter)));
	}
	// _monitoringEvent();
}

Server *ServerManager::_addServer(int port)
{
	Server *val;
	std::map<std::string, std::string>::iterator it;

	val = new Server();
	for (int i = 0; i < _conf.getServerInfos().size(); i++)
		for (it = _conf.getServerInfos()[i]->getServerInfo().begin(); it != _conf.getServerInfos()[i]->getServerInfo().end(); it++)
			if (it->second == "listen")
				if (atoi((it->first).c_str()) == port)
					val->setHostServer(_conf.getServerInfos()[i]);
	return (val);
}

// void ServerManager::_monitoringEvent()
// {
// 	int numEvents;
// 	struct kevent *event;

// 	while (true)
// 	{
// 		numEvents = _kqueue.doKevent();
// 		std::cout << numEvents << std::endl;
// 		for (int i = 0; i < numEvents; i++)
// 		{
// 			event = &_kqueue.getEventList()[i];
// 			eFdType type = _kqueue.getFdType(event->ident);
// 			if (event->flags & EV_ERROR)
// 			{
// 				switch (type)
// 				{
// 				case SERVER:
// 					std::cerr << "server socket error!" << std::endl;
// 					break;
// 				case CLIENT:
// 					_kqueue.deleteEvent(event->ident);
// 					break;
// 				default:
// 					break;
// 				}
// 			}
// 			else if (event->flags & EVFILT_READ)
// 			{
// 				switch (type)
// 				{
// 				case SERVER:
// 					_acceptClient(event->ident);
// 					break;
// 				case CLIENT:
// 					_readRequest(event->ident);
// 					break;
// 				default:
// 					break;
// 				}
// 			}
// 			else if (event->flags & EVFILT_WRITE)
// 			{
// 				_writeResponse(event->ident);
// 			}
// 		}
// 	}
// }

// void ServerManager::_acceptClient(uintptr_t servSock)
// {
// 	uintptr_t clntSock;
// 	struct sockaddr_in serv;

// 	if ((clntSock = accept(servSock, NULL, NULL)) == -1)
// 		Exception::acceptError("accept() error!");
// 	fcntl(clntSock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
// 	_kqueue.addEvent(clntSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
// 	_kqueue.addEvent(clntSock, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
// 	_kqueue.setFdset(clntSock, CLIENT);
// }

// void _readRequest(uintptr_t clntSock)
// {
// 	// 리퀘스트를 담을 버퍼를 ServerManager 가 들고 있어야 하나? -> 서버로 넘겨야 되지 않을까 싶음. 서버 객체들을 서버매니저가 가지고 있고 각 서버 객체가 request를 가지고 있는 식이 좋지 않을까.
//  // 문제점 하나 더, client socket만 인자로 받는 경우 해당 socket이 어떤 server socket으로 들어왔는지 알 수 가 있나...?
// // 버퍼는 어떤 자료형을 쓰지? clntSock, buf 를 매핑? == map? 아님 그냥 하나의 string?
// }

// void _writeResponse(uintptr_t clntSock) {}
