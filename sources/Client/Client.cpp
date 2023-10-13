#include "Client.hpp"

Client::Client(uintptr_t socket)
	: _status(START), _socket(socket), _cgi(NULL) {}

Client::Client(const Client &src)
	: _status(START), _socket(src._socket), _cgi(NULL) {}

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
	bool flag = false;
	Config &conf = Config::getInstance();
	ssize_t len;
	std::string buf;

	if (_status == START)
		_status = READHEADER;

	buf.clear();
	buf.resize(conf.getClientHeadBufferSize());

	if ((len = recv(_socket, &buf[0], conf.getClientHeadBufferSize(), 0)) == -1)
		Exception::recvError("recv() error!");
	else if (len <= 0)
		Exception::disconnectDuringRecvError("disconnected during read!");
	std::cout << "***************************" << std::endl;
	std::cout << "BUF : " << buf << std::endl;
	std::cout << "***************************" << std::endl;
	if (_status == READHEADER)
		_request.setHeaderBuf(buf.c_str());
	if (_request.getIsBody() == true && _status == READHEADER)
	{
		_request.parseRequest();
		if (_request.getParsedRequest()._transferEncoding == "chunked")
		{
			_status = READCHUNKEDBODY;
			_chunkedBodyBuf = _request.getParsedRequest()._body;
			_request.getParsedRequest()._body.clear();
			flag = true;
		}
		else
			_status = READBODY;
	}
	else if (_status == READBODY)
		_request.setBodyBuf(buf.c_str());
	if (_status == READCHUNKEDBODY)
	{
		if (flag == false)
			_chunkedBodyBuf.append(buf);
		size_t size = std::strtol(_chunkedBodyBuf.c_str(), NULL, 16);
		std::string::size_type pos;
		std::cout << "###########################" << std::endl;
		std::cout << "CHUNKED BODY BUF : " << _chunkedBodyBuf << std::endl;
		std::cout << "SIZE : " << size << std::endl;
		std::cout << "###########################" << std::endl;
		if (size == 0 && _chunkedBodyBuf.find("\r\n\r\n") != std::string::npos)
		{
			_status = FINREAD;
			_chunkedBodyBuf.clear();
			return;
		}
		pos = _chunkedBodyBuf.find("\r\n");
		if (pos != std::string::npos)
		{
			if (_chunkedBodyBuf.length() - (pos + 2) >= size && size != 0)
			{
				// std::cout << "SIZE : " << size << std::endl;
				// std::cout << "POS : " << pos + 2 << std::endl;
				// std::cout << "CHUNKED BODY LENGTH : " << _chunkedBodyBuf.length() << std::endl;
				// std::cout << "CHUNKED BODY BUF : " << _chunkedBodyBuf << std::endl;
				_request.setChunkedBodyBuf(_chunkedBodyBuf.substr(pos + 2, _chunkedBodyBuf.find("\r\n", pos + 2)));
				_chunkedBodyBuf = _chunkedBodyBuf.substr(pos + 2 + size, _chunkedBodyBuf.length());
				std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
				std::cout << "CHUNKED BODY BUF : " << _chunkedBodyBuf << std::endl;
				std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
				sleep(5);
			}
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
	try
	{
		if (_response.isAllowedMethod(_request.getParsedRequest()._method) == false)
		{
			throw(_405_METHOD_NOT_ALLOWED);
		}
		else if (_response.isCgi() == true)
		{
			if (_response.getResponse() == "")
			{
				Kqueue &kq = Kqueue::getInstance();

				_cgi = new Cgi(&_request, &_response, _socket, this);
				_cgi->cgiStart();
				kq.disableEvent(_socket, EVFILT_WRITE, static_cast<void *>(this));
				return;
			}
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
		std::string &response = _response.getResponse();
		_status = FINWRITE;
		std::cout << _response.getResponse();
		send(_socket, static_cast<void *>(&response[0]), response.size(), 0);
	}
	catch (const eStatus &e)
	{
		std::map<std::string, std::string> tmp = _response.getServerInfo()->getServerInfo();
		std::string findResult = mapFind(tmp, "errorpage");

		if (findResult.empty() == false)
			findResult = "." + findResult;
		_status = FINWRITE;

		sendErrorPage(_socket, findResult, e);
	}
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
		pos = getNextPos(pos);
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
	_request.initRequest();
	_response.initResponse();
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
