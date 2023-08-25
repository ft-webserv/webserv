#ifndef Server_HPP
#define Server_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

class Server
{
public:
	Server(void);
	Server(const Server &src);
	Server &operator=(Server const &rhs);
	virtual ~Server(void);

private:
	const int socketId;
	const int port;
	struct sockaddr_in serverAddr;
};

#endif
