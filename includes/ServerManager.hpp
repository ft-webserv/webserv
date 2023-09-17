#pragma once

// standard header
#include <cstring>
#include <sys/socket.h>

// user define header
#include "Config.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Kqueue.hpp"

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
	void _monitoringEvent(void);
	void _acceptClient(uintptr_t &servSock);
	void _findServerBlock(Client *client);
};
