#ifndef ServerManager_HPP
#define ServerManager_HPP

#include "Server.hpp"
#include "Config.hpp"

class ServerManager
{
public:
	ServerManager(Config &conf);
	ServerManager(const ServerManager &src);
	ServerManager &operator=(ServerManager const &rhs);
	virtual ~ServerManager(void);

private:
	Config &_conf;
	std::map<std::string, Server *> _servers;
	void openServSock(void);
	void addServer(void);
	Server *setServer(void);
};

#endif
