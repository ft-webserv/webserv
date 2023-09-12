#ifndef Server_HPP
#define Server_HPP

#include "Config.hpp"
#include "Request.hpp"
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
	Server(port_t port);
	Server(const Server &src);
	Server &operator=(Server const &rhs);
	virtual ~Server(void);
	void makeResponse(Request *request);

public:
	void setHostServer(ServerInfo *serverBlock);
	std::map<std::string, ServerInfo *> getHostServer();
	void setDefaultServer(ServerInfo *serverBlock);
	ServerInfo *getDefaultServer();

private:
	std::map<std::string, ServerInfo *> _hostServer;
	ServerInfo *_defaultServer;
};

#endif
