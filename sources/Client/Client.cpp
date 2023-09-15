#include "Client.hpp"

Client::Client(uintptr_t socket)
	: _socket(socket) {}

Client::Client(const Client &src)
	: _socket(src._socket) {}

Client &Client::operator=(const Client &src)
{
	return (*this);
}

Client::~Client()
{
}

void Client::readRequest(intptr_t data)
{
	ssize_t len;
	char buf[BUFFERSIZE + 1];

	if ((len = recv(_socket, buf, BUFFERSIZE, 0)) == -1)
		Exception::recvError("recv() error!");
	else if (len <= 0)
		Exception::disconnectDuringRecvError("diconnected during read!");
	_request.setRequestBufs(buf);
	memset(buf, 0, BUFFERSIZE + 1);
	if (data <= BUFFERSIZE)
		_request.parseRequest();
}
