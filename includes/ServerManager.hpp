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
	std::map<std::string, Server *> _servers;
	void _addServer(void);
	Server *_setServer(void);
	void _monitoringEvent(void);
	void _acceptClient(uintptr_t servSock);
};

#endif
