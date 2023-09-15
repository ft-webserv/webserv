#include "Server.hpp"

Server::Server(port_t port)
{
	std::map<std::string, std::string>::iterator it;
  Config& _conf = Config::getInstance();

	_defaultServer = NULL;
	for (int i = 0; i < _conf.getServerInfos().size(); i++)
	{
		for (it = _conf.getServerInfos()[i]->getServerInfo().begin(); it != _conf.getServerInfos()[i]->getServerInfo().end(); it++)
		{
			if (it->second == "listen")
			{
				if (atoi((it->first).c_str()) == port)
				{
					if (_defaultServer == NULL)
						_defaultServer = _conf.getServerInfos()[i];
					setHostServer(_conf.getServerInfos()[i]);
				}
			}
		}
	}
}

Server::Server(const Server &src)
{
}

Server::~Server(void)
{
}

void Server::makeResponse(Request *request, uintptr_t clntSock)
{
	Response response;
	ServerInfo *servInfo;
	std::map<std::string, ServerInfo *>::iterator it;

	if ((it = _hostServer.find(request->getParsedRequest()._host)) != _hostServer.end())
		servInfo = it->second;
	else
		servInfo = _defaultServer;
	if (request->getParsedRequest()._method == "GET") // http request의 location에 정규표현식이 들어올 수 있는가?
	{
		response.handleGet(servInfo, request);
	}
	else if (request->getParsedRequest()._method == "POST")
	{
	}
	else if (request->getParsedRequest()._method == "DELETE")
	{
	}
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
