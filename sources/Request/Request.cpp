
#include "Request.hpp"

Request::Request()
{
}

Request::~Request()
{
}

void ServerManager::_readRequest(uintptr_t &clntSock, intptr_t data)
{
	ssize_t len;
	char buf[BUFFERSIZE + 1];

	if ((len = recv(clntSock, buf, BUFFERSIZE, 0)) == -1)
		Exception::recvError("recv() error!");
	else if (len <= 0)
		Exception::disconnectDuringRecvError("diconnected during read!");
	_clientBufs.find(clntSock)->second.append(buf);
	memset(buf, 0, BUFFERSIZE + 1);
	if (data <= BUFFERSIZE)
	{
		struct sockaddr_in clnt;
		socklen_t clntSockLen = sizeof(clnt);
		port_t port;

		getsockname(clntSock, (sockaddr *)&clnt, &clntSockLen);
		port = ntohs(clnt.sin_port);
		std::string request = _clientBufs.find(clntSock)->second;
		(_servers.find(port)->second)->parseRequest(request);
	}
}
