#ifndef Server_HPP
#define Server_HPP

#include "Config.hpp"
// socket(), bind(), listen(), accept()
#include <sys/socket.h>
// sockaddr_in struct
#include <arpa/inet.h>
// fcntl()
#include <fcntl.h>
#include <map>
#define BACKLOG SOMAXCONN

typedef struct s_request
{
	std::string _method;
	std::string _location;
	std::string _httpVersion;
	std::string _host;
	std::string _accept;
	std::string _connection;
	std::string _contentType;
	unsigned int _contentLength;
	std::string _body;
} t_request;

class Server
{
public:
	Server(void);
	Server(port_t port);
	Server(const Server &src);
	Server &operator=(Server const &rhs);
	virtual ~Server(void);
	void setHostServer(ServerInfo *serverBlock);
	std::map<std::string, ServerInfo *> getHostServer();
	void parseRequest(std::string request);
	void testPrintRequest(void);

private:
	std::map<std::string, ServerInfo *> _hostServer;
	t_request _requestInfo;
};

#endif
