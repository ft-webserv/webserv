#ifndef Server_HPP
#define Server_HPP

// socket(), bind(), listen(), accept()
#include <sys/socket.h>
// sockaddr_in struct
#include <arpa/inet.h>
// fcntl()
#include <fcntl.h>
#define BACKLOG SOMAXCONN

class Server
{
public:
	Server(void);
	Server(const Server &src);
	Server &operator=(Server const &rhs);
	virtual ~Server(void);

private:
	int _socketId;
	int _port;
	struct sockaddr_in _serverAddr;
};

#endif
