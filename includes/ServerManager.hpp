#ifndef ServerManager_HPP
#define ServerManager_HPP

// standard header
#include <cstring>

// user define header
#include "Server.hpp"
#include "Config.hpp"
#include "Kqueue.hpp"
#include "Request.hpp"

// user define value
#define BUFFERSIZE 1024

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
	std::map<port_t, Server *> _servers;
	std::map<uintptr_t, Request *> _requests;
	Server *_addServer(port_t port);
	void _monitoringEvent(void);
	void _acceptClient(uintptr_t &servSock);
	void _readRequest(uintptr_t &clntSock, intptr_t data);
	// void _writeResponse(uintptr_t clntSock);
};

#endif
