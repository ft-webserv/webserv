#pragma once

#include <cstring>
#include <sys/socket.h>

// user define header
#include "Config.hpp"
#include "Client.hpp"
#include "Kqueue.hpp"
#include "Logger.hpp"

#define BACKLOG SOMAXCONN
class ServerManager
{
public:
	ServerManager();
	ServerManager(const ServerManager &src);
	ServerManager &operator=(ServerManager const &rhs);
	~ServerManager(void);
	void runServer();

private:
	Kqueue &_kqueue;
	void _monitoringEvent(void);
	void _acceptClient(uintptr_t &servSock);
	void _findServerBlock(Client *client);
	void _setKeepAliveTimeOut(Client *client);
	void _setRequestTimeOut(Client *client);
	port_t _findOutPort(uintptr_t clntsock);
	void _disconnectClient(Client *client);

	void _handleErrorFlag(eFdType type, void *udata);
	void _handleReadFilter(eFdType type, struct kevent *event);
	void _handleWriteFilter(eFdType type, struct kevent *event);
	void _handleTimerFilter(struct kevent *event);
	void _handleReadClient(struct kevent *event);
	void _handleWriteClient(struct kevent *event);

	void _catchKnownError(const eStatus &e, eFdType type, struct kevent *event);
};
