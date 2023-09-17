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

void Client::writeResponse()
{

	if (_request.getParsedRequest()._method == "GET") // http request의 location에 정규표현식이 들어올 수 있는가?
	{
		_response.handleGet(_request);
	}
	else if (_request.getParsedRequest()._method == "POST")
	{
		_response.handlePost(_request);
	}
	else if (_request.getParsedRequest()._method == "DELETE")
	{
		_response.handleDelete(_request);
	}
}

uintptr_t Client::getSocket()
{
	return (_socket);
}

void Client::setServerBlock(port_t port)
{
	std::map<std::string, std::string>::iterator it;
	Config &conf = Config::getInstance();

	for (int i = 0; i < conf.getServerInfos().size(); i++)
	{
		int flag = 0;
		for (it = conf.getServerInfos()[i]->getServerInfo().begin(); it != conf.getServerInfos()[i]->getServerInfo().end(); it++)
		{
			if (it->second == "listen")
			{
				if (atoi((it->first).c_str()) == port)
				{
					if (_response.getServerInfo() == NULL)
						_response.setServerInfo(conf.getServerInfos()[i]);
					flag = 1;
				}
			}
			if (flag == 1 && it->second == "server_name")
			{
				if (it->first == _request.getParsedRequest()._host)
				{
					_response.setServerInfo(conf.getServerInfos()[i]);
					setLocationBlock();
					return;
				}
			}
		}
	}
}

void Client::setLocationBlock()
{
	std::vector<LocationInfo *>::iterator it = _response.getServerInfo()->getLocationInfos().begin();
	std::vector<LocationInfo *>::iterator end = _response.getServerInfo()->getLocationInfos().end();
	std::string token;
	std::string path;
	std::string::size_type pos = 0;
	std::string::size_type tokenLength;

	while (true)
	{
		pos = getNextPos(pos);
		if (pos == std::string::npos)
			break;
		token = _request.getParsedRequest()._location.substr(0, pos);
		tokenLength = token.length();
		for (; it != end; it++)
		{
			path = (*it)->getPath();
			if (path.length() + 1 == tokenLength && token[tokenLength - 1] == '/')
				tokenLength -= 1;
			if (path.compare(0, token.length(), token) == 0)
				_response.setLocationInfo(*it);
		}
	}
}

std::string::size_type Client::getNextPos(std::string::size_type currPos)
{
	std::string location = _request.getParsedRequest()._location;
	std::string::size_type nextPos;

	nextPos = location.find("/", currPos) + 1;
	if (nextPos == std::string::npos)
		nextPos = location.length();
	if (currPos == nextPos)
		return (std::string::npos);
	return (nextPos);
}
