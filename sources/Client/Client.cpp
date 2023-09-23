#include "Client.hpp"

Client::Client(uintptr_t socket)
	: _socket(socket), _status(START) {}

Client::Client(const Client &src)
	: _socket(src._socket), _status(START) {}

Client &Client::operator=(const Client &src)
{
	return (*this);
}

Client::~Client()
{
}

eClientStatus &Client::getClientStatus()
{
	return (_status);
}

uintptr_t &Client::getSocket()
{
	return (_socket);
}

Request &Client::getRequest()
{
	return (_request);
}

Response &Client::getResponse()
{
	return (_response);
}

void Client::readRequest()
{
	ssize_t len;
	char buf[BUFFERSIZE + 1];

	if (_status == START)
		_status = READHEADER;
	else if (_status == READBODY && _request.getParsedRequest()._contentLength == _request.getParsedRequest()._body.length())
		_status = FINREAD;

	memset(buf, 0, BUFFERSIZE + 1);
	if ((len = recv(_socket, buf, BUFFERSIZE, 0)) == -1)
		Exception::recvError("recv() error!");
	else if (len <= 0)
		Exception::disconnectDuringRecvError("diconnected during read!");

	if (_status == READHEADER)
		_request.setHeaderBuf(buf);
	else if (_status == READBODY)
		_request.setBodyBuf(buf);

	if (_request.getIsBody() == true && _status != READBODY)
	{
		_status = READBODY;
		_request.parseRequest();
	}
}

void Client::writeResponse()
{
	if (chdir(WORK_PATH) == -1)
		Exception::listenError("chdir() error!");
	try
	{
		if (_request.getParsedRequest()._method == "GET")
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
		else
		{
			throw(_501_NOT_IMPLEMENTED);
		}
		std::string &response = _response.getResponse();
		send(_socket, static_cast<void *>(&response[0]), response.size(), 0);
	}
	catch (const eStatus &e)
	{
		std::map<std::string, std::string> tmp = _response.getServerInfo()->getServerInfo();
		std::string findResult = mapFind(tmp, "errorpage");

		if (findResult.empty() == false)
			findResult = "." + findResult;
		_sendErrorPage(_socket, findResult, e);
	}
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
