#ifndef ServerManager_HPP
#define ServerManager_HPP

#include "Server.hpp"
#include "Config.hpp"
#include <map>
#include <string>

class ServerManager
{
public:
	ServerManager(void);
	ServerManager(const ServerManager &src);
	ServerManager &operator=(ServerManager const &rhs);
	virtual ~ServerManager(void);

private:
	std::map<std::string, Server> Servers;
	void addServer(/* arguments */);
	void setServer(char *fileName);
};

#endif
