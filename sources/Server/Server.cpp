#include "Server.hpp"

Server::Server(void)
{
}

Server::Server(int port)
{
}

Server::Server(const Server &src)
{
}

Server::~Server(void)
{
}

void Server::setHostServer(ServerInfo *serverBlock)
{
	std::map<std::string, std::string>::iterator it;
	for (it = serverBlock->getServerInfo().begin(); it != serverBlock->getServerInfo().end(); it++)
	{
		if (it->second == "server_name")
		{
			_hostServer.insert(std::pair<std::string, ServerInfo *>(it->first, serverBlock));
			return;
		}
	}
}

void parseRequest(std::string request)
{
}

std::map<std::string, ServerInfo *> Server::getHostServer()
{
	return (_hostServer);
}
