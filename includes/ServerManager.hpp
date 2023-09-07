#ifndef ServerManager_HPP
#define ServerManager_HPP

// standard header
#include <cstring>

// user define header
#include "Server.hpp"
#include "Config.hpp"
#include "Kqueue.hpp"

class ServerManager
{
public:
	ServerManager(Config &conf);
	ServerManager(const ServerManager &src);
	ServerManager &operator=(ServerManager const &rhs);
	virtual ~ServerManager(void);
	void runServer();

private:
	Kqueue _kqueue;
	Config &_conf;
	std::map<uintptr_t, Server *> _servers;
	Server *_addServer(int port);
	void _monitoringEvent(void);
	void _acceptClient(uintptr_t servSock);
	void _readRequest(uintptr_t clntSock);
	void _writeResponse(uintptr_t clntSock);
};

#endif
