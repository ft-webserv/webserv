
#include "Server.hpp"

Server::Server(void)
{
}

Server::Server(port_t port)
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

void Server::testPrintRequest(void)
{
	std::cout << "Method: " << _requestInfo._method << std::endl;
	std::cout << "Location: " << _requestInfo._location << std::endl;
	std::cout << "HTTPVersion: " << _requestInfo._httpVersion << std::endl;
	std::cout << "HOST: " << _requestInfo._host << std::endl;
	std::cout << "Accept: " << _requestInfo._accept << std::endl;
	std::cout << "Connection: " << _requestInfo._connection << std::endl;
	std::cout << "ContentType: " << _requestInfo._contentType << std::endl;
	std::cout << "ContentLength: " << _requestInfo._contentLength << std::endl;
	std::cout << "Body: " << _requestInfo._body << std::endl;
}

void Server::parseRequest(std::string request)
{
	std::string::size_type pos;
	std::string word;

	memset(&_requestInfo, 0, sizeof(_requestInfo));
	for (pos = 0; pos < request.length();)
	{
		std::string::size_type pre = pos;

		pos = request.find("\r\n", pos);
		std::istringstream line(request.substr(pre, pos));
		line >> word;
		if (word == "GET" || word == "POST" || word == "DELETE")
		{
			_requestInfo._method = word;
			line >> _requestInfo._location >> _requestInfo._httpVersion;
		}
		else if (word == "Host:")
		{
			line >> _requestInfo._host;
		}
		else if (word == "Accept:")
		{
			line >> _requestInfo._accept;
		}
		else if (word == "Connection:")
		{
			line >> _requestInfo._connection;
		}
		else if (word == "Content-Type:")
		{
			line >> _requestInfo._contentType;
		}
		else if (word == "Content-Length:")
		{
			line >> _requestInfo._contentLength;
		}
		else if (word == "")
		{
			_requestInfo._body = request.substr(pos, request.length());
		}
		pos += 2;
		testPrintRequest();
	}
}

std::map<std::string, ServerInfo *> Server::getHostServer()
{
	return (_hostServer);
}
