#pragma once

// standard header
#include <cstring>
#include <sys/socket.h>

// user define header
#include "Server.hpp"
#include "Config.hpp"
#include "Kqueue.hpp"

// user define value
#define BUFFERSIZE 1024

class ServerManager
{
public:
	ServerManager();
	ServerManager(const ServerManager &src);
	ServerManager &operator=(ServerManager const &rhs);
	~ServerManager(void);
	void runServer();

private:
	Kqueue _kqueue;
	std::map<port_t, Server *> _servers;
	std::map<uintptr_t, Request *> _requests;
	void _monitoringEvent(void);
	void _acceptClient(uintptr_t &servSock);
	void _readRequest(uintptr_t &clntSock, intptr_t data);
	void _writeResponse(uintptr_t clntSock);
};
