#pragma once

#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Status.hpp"
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
	Server(const Server &src);
	Server &operator=(Server const &rhs);
	virtual ~Server(void);
};
