#ifndef Server_HPP
#define Server_HPP

#include "ServerInfo.hpp"
// socket(), bind(), listen(), accept()
#include <sys/socket.h>
// sockaddr_in struct
#include <arpa/inet.h>
// fcntl()
#include <fcntl.h>
#include <map>
#define BACKLOG SOMAXCONN

class Server
{
public:
	Server(void);
	Server(int port);
	Server(const Server &src);
	Server &operator=(Server const &rhs);
	virtual ~Server(void);
	void setHostServer(ServerInfo *serverBlock);
	std::map<std::string, ServerInfo *> getHostServer();
	int getPort();

private:
	std::map<std::string, ServerInfo *> _hostServer;
};

#endif
