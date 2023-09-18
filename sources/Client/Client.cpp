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
	if (chdir(ABSOLUTE_PATH) == -1)
		Exception::listenError("chdir() error!");
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
	std::string &response = _response.getResponse();
	send(_socket, static_cast<void *>(&response[0]), response.size(), 0);
}

uintptr_t Client::getSocket()
{
	return (_socket);
}

void Client::setServerBlock(port_t port)
{
	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string>::iterator end;
	Config &conf = Config::getInstance();

	for (int i = 0; i < conf.getServerInfos().size(); i++)
	{
		int flag = 0;
		it = conf.getServerInfos()[i]->getServerInfo().begin();
		end = conf.getServerInfos()[i]->getServerInfo().end();
		for (; it != end; it++)
		{
			if (it->second == "listen")
			{
				if (atoi((it->first).c_str()) == port)
				{
					if (_response.getServerInfo() == NULL)
					{
						_response.setServerInfo(conf.getServerInfos()[i]);
						setLocationBlock();
					}
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
	std::vector<LocationInfo *>::iterator it;
	std::vector<LocationInfo *>::iterator end;
	std::string token;
	std::string path;
	std::string::size_type pos = 0;
	std::string::size_type pathLength;
	std::string::size_type tokenLength;

	it = _response.getServerInfo()->getLocationInfos().begin();
	end = _response.getServerInfo()->getLocationInfos().end();
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
			pathLength = path.length();
			if (pathLength > 1 && path[pathLength - 1] == '/')
				pathLength -= 1;
			if (tokenLength > 1 && token[tokenLength - 1] == '/')
				tokenLength -= 1;
			if (pathLength == tokenLength && path.compare(0, pathLength, token) == 0)
				_response.setLocationInfo(*it);
		}
	}
}

std::string::size_type Client::getNextPos(std::string::size_type currPos)
{
	std::string location = _request.getParsedRequest()._location;
	std::string::size_type nextPos;

	nextPos = location.find("/", currPos);
	if (nextPos == std::string::npos)
		nextPos = location.length();
	else
		nextPos += 1;
	if (currPos == nextPos)
		return (std::string::npos);
	return (nextPos);
}
