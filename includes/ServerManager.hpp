#ifndef ServerManager_HPP
#define ServerManager_HPP

// standard header
#include <cstring>

// user define header
#include "Server.hpp"
#include "Config.hpp"
#include "Kqueue.hpp"
#include "Exception.hpp"

class ServerManager
{
public:
	ServerManager(Config &conf);
	ServerManager(const ServerManager &src);
	ServerManager &operator=(ServerManager const &rhs);
	virtual ~ServerManager(void);
	int newClientSock();
	int newServSock();

private:
	Kqueue _kqueue;
	Config &_conf;
	std::map<std::string, Server *> _servers;
	void _addServer(void);
	Server *_setServer(void);
};

#endif
