#include "Server.hpp"

Server::Server(void)
{
	_defaultServer = NULL;
}

Server::Server(port_t port)
{
	_defaultServer = NULL;
}

Server::Server(const Server &src)
{
}

Server::~Server(void)
{
}

void Server::makeResponse(Request *request)
{
	ServerInfo *serverInfo;
	std::map<std::string, ServerInfo *>::iterator it;

	if ((it = _hostServer.find(request->getParsedRequest()._host)) != _hostServer.end())
		serverInfo = it->second;
	else
		serverInfo = _defaultServer;

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

std::map<std::string, ServerInfo *> Server::getHostServer()
{
	return (_hostServer);
}

void Server::setDefaultServer(ServerInfo *serverBlock)
{
	_defaultServer = serverBlock;
}

ServerInfo *Server::getDefaultServer()
{
	return (_defaultServer);
}
