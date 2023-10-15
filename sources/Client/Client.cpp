#include "Client.hpp"

Client::Client(uintptr_t socket)
	: _lastPos(0), _status(START), _socket(socket), _cgi(NULL) {}

Client::Client(const Client &src)
	: _lastPos(0), _status(START), _socket(src._socket), _cgi(NULL) {}

Client &Client::operator=(const Client &src)
{
	(void)src;
	return (*this);
}

Client::~Client()
{
}

eClientStatus &Client::getClientStatus() { return (_status); }
uintptr_t &Client::getSocket() { return (_socket); }
Request &Client::getRequest() { return (_request); }
Response &Client::getResponse() { return (_response); }
Cgi *Client::getCgi() { return (_cgi); }
void Client::setCgi(Cgi *cgi) { _cgi = cgi; }
// bool Client::getIsCgi() { return (_isCgi); }

void Client::readRequest(struct kevent *event)
{
	Config &conf = Config::getInstance();
	ssize_t len;
	std::string buf;

	if (_status == START)
		_status = READHEADER;

	buf.clear();
	buf.resize(event->data);

	if ((len = recv(_socket, &buf[0], event->data, 0)) == -1)
		Exception::recvError("recv() error!");
	else if (len <= 0)
		Exception::disconnectDuringRecvError("disconnected during read()");
	// std::cout << "***************************" << std::endl;
	// std::cout << "BUF : " << buf << std::endl;
	// std::cout << "***************************" << std::endl;
	if (_status == READHEADER)
		buf = _request.setHeaderBuf(buf);
	if (_request.getIsBody() == true && _status == READHEADER)
	{
		_request.parseRequest();
		if (_request.getParsedRequest()._transferEncoding == "chunked")
		{
			_status = READCHUNKEDBODY;
			_chunkedBodyBuf = _request.getParsedRequest()._body;
			_request.getParsedRequest()._body.clear();
		}
		else
			_status = READBODY;
	}
	if (_status == READBODY)
		_request.setBodyBuf(buf);
	else if (_status == READCHUNKEDBODY)
	{
		_chunkedBodyBuf.append(buf.c_str());
		while (1)
		{
			std::string::size_type pos;
			std::size_t size = -1;

			// std::cout << "CHUNKED BODY: " << _chunkedBodyBuf << std::endl;
			// std::cout << "CHUNKED BODY SIZE : " << _chunkedBodyBuf.length() << std::endl;
			pos = _chunkedBodyBuf.find("\r\n");
			if (pos == std::string::npos)
				break;
			else
				size = std::strtol(_chunkedBodyBuf.c_str(), NULL, 16);

			if (size == 0 && _chunkedBodyBuf.find("\r\n\r\n") != std::string::npos)
			{
				_status = FINREAD;
				std::cout << _request.getParsedRequest()._body.length() << std::endl;
				std::cout << "CHUNKEDBODY : " << _chunkedBodyBuf << std::endl;
				_chunkedBodyBuf.clear();
				return;
			}
			else if (_chunkedBodyBuf.length() - (pos + 2) >= size + 2 && size != 0)
			{
				_chunkedBodyBuf = _chunkedBodyBuf.substr(pos + 2);
				_request.setChunkedBodyBuf(_chunkedBodyBuf.substr(0, size));
				_chunkedBodyBuf = _chunkedBodyBuf.substr(size + 2);
			}
			else
				break;
		}
	}
	if (_request.getParsedRequest()._body.size() > conf.getClientMaxBodySize())
		throw(_413_REQUEST_ENTITY_TOO_LARGE);
	if (_status == READBODY && _request.getParsedRequest()._contentLength == _request.getParsedRequest()._body.length())
		_status = FINREAD;
}

void Client::writeResponse()
{
	// if (chdir(WORK_PATH) == -1)
	// 	Exception::listenError("chdir() error!");
	if (_response.isAllowedMethod(_request.getParsedRequest()._method) == false)
	{
		throw(_405_METHOD_NOT_ALLOWED);
	}
	else if (_status != CGISTART && _response.isCgi(_request.getParsedRequest()._location) == true)
	{
		Kqueue &kq = Kqueue::getInstance();

		_cgi = new Cgi(&_request, &_response, _socket, this);
		_cgi->cgiStart();
		kq.disableEvent(_socket, EVFILT_WRITE, static_cast<void *>(this));
		_status = CGISTART;
		return;
	}
	else if (_status == CGISTART)
	{
		_status = CGIFIN;
	}
	else if (_request.getParsedRequest()._method == "GET" || _request.getParsedRequest()._method == "HEAD")
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
	_sendResponse();
}

void Client::_sendResponse()
{
	size_t sendSize = 2048;
	std::string &response = _response.getResponse();

	if (response.length() - _lastPos < sendSize)
		sendSize = response.length() - _lastPos;
	ssize_t res = send(_socket, response.c_str() + _lastPos, sendSize, 0);
	if (res == -1)
		throw(_500_INTERNAL_SERVER_ERROR);
	else if (res <= 0)
		Exception::disconnectDuringSendError("disconnected during send()");
	if (static_cast<size_t>(res) < response.length() - _lastPos)
	{
		_lastPos += res;
		std::cout << _lastPos << std::endl;
		return;
	}
	std::cout << ")))))))))))))))))))))" << std::endl;
	std::cout << response.size() << std::endl;
	std::cout << response << std::endl;
	_status = FINWRITE;
}

void Client::setServerBlock(port_t port)
{
	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string>::iterator end;
	Config &conf = Config::getInstance();

	for (size_t i = 0; i < conf.getServerInfos().size(); i++)
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

	end = _response.getServerInfo()->getLocationInfos().end();
	while (true)
	{
		pos = _getNextPos(pos);
		if (pos == std::string::npos)
			break;
		token = _request.getParsedRequest()._location.substr(0, pos);
		it = _response.getServerInfo()->getLocationInfos().begin();
		tokenLength = token.length();
		for (; it != end; it++)
		{
			path = (*it)->getPath();
			pathLength = path.length();
			if (pathLength > 1 && path[pathLength - 1] == '/')
				pathLength -= 1;
			if (tokenLength > 1 && token[tokenLength - 1] == '/')
				tokenLength -= 1;
			if (pathLength == tokenLength && token.compare(0, tokenLength, path) == 0)
				_response.setLocationInfo(*it);
		}
	}
}

void Client::initClient()
{
	_status = START;
	_cgi = NULL;
	_lastPos = 0;
	_request.initRequest();
	_response.initResponse();
}

std::string::size_type Client::_getNextPos(std::string::size_type currPos)
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
