#include "Request.hpp"

Request::Request()
	: _isBody(false)
{
	_parsedRequest._contentLength = 0;
}

Request::~Request()
{
}

void Request::initRequest()
{
	_headerBuf.clear();
	_isBody = false;
	_parsedRequest._method.clear();
	_parsedRequest._location.clear();
	_parsedRequest._httpVersion.clear();
	_parsedRequest._host.clear();
	_parsedRequest._accept.clear();
	_parsedRequest._connection.clear();
	_parsedRequest._contentType.clear();
	_parsedRequest._transferEncoding.clear();
	_parsedRequest._contentLengthStr.clear();
	_parsedRequest._body.clear();
	_parsedRequest._contentLength = 0;
}

void Request::setHeaderBuf(const char *buf)
{
	std::string::size_type newLinePos;

	_headerBuf += buf;
	if ((newLinePos = _headerBuf.find("\r\n\r\n")) != std::string ::npos)
	{
		_isBody = true;
		_parsedRequest._body = _headerBuf.substr(newLinePos + 4, _headerBuf.length());
		// std::cout << "pos :" << newLinePos << "headerbuf :" << _headerBuf.length() << std::endl;
		_headerBuf = _headerBuf.substr(0, newLinePos) + "\r\n";
	}
}

void Request::setBodyBuf(const char *buf)
{
	_parsedRequest._body += buf;
}

void Request::setChunkedBodyBuf(const std::string buf)
{
	Config &conf = Config::getInstance();

	if (_parsedRequest._body.size() > conf.getClientMaxBodySize())
		throw(_413_REQUEST_ENTITY_TOO_LARGE);
	_parsedRequest._body += buf;
}

t_request Request::getParsedRequest() const
{
	return (_parsedRequest);
}

bool Request::getIsBody()
{
	return (_isBody);
}

void Request::parseRequest()
{
	std::string::size_type pos = 0;
	std::string::size_type pre = 0;
	std::string word;

	if (_isBody == true)
	{
		pos = _headerBuf.find("\r\n");
		_parseStartLine(pre, pos);
		for (; pos < _headerBuf.length();)
		{
			std::istringstream line(_headerBuf.substr(pre, pos - pre));
			line >> word;
			if (word == "Host:")
			{
				if (_parsedRequest._host.empty() == false)
					throw(_400_BAD_REQUEST);
				line >> _parsedRequest._host;
			}
			else if (word == "Accept:")
			{
				line >> _parsedRequest._accept;
			}
			else if (word == "Connection:")
			{
				line >> _parsedRequest._connection;
			}
			else if (word == "Content-Type:")
			{
				line >> _parsedRequest._contentType;
			}
			else if (word == "Content-Length:")
			{
				std::string tmp;

				line >> tmp;
				if (_parsedRequest._contentLengthStr == "")
					_parsedRequest._contentLengthStr = tmp;
				else if (_parsedRequest._contentLengthStr != tmp)
					throw(_400_BAD_REQUEST);
				if (_parsedRequest._contentLengthStr.find_first_not_of("0123456789") != std::string::npos)
					throw(_400_BAD_REQUEST);
				else if (_parsedRequest._contentLengthStr.length() >= 10)
					throw(_413_REQUEST_ENTITY_TOO_LARGE);
				_parsedRequest._contentLength = ft_stoi(_parsedRequest._contentLengthStr);
			}
			else if (word == "Transfer-Encoding:")
			{
				int num = 0;
				while (1)
				{
					line >> _parsedRequest._transferEncoding;
					if (line.eof() == true)
						break;
					num++;
					if (num > 1)
						throw(_501_NOT_IMPLEMENTED);
				}
			}
			else
			{
				if (word.find(":") == std::string::npos)
					throw(_400_BAD_REQUEST);
			}
			pos += 2;
			pre = pos;
			pos = _headerBuf.find("\r\n", pos);
		}
		_checkValidHeader();
	}
	// testPrintRequest();
}

void Request::_parseStartLine(std::string::size_type &pre, std::string::size_type &pos)
{
	std::istringstream line(_headerBuf.substr(pre, pos - pre));

	line >> _parsedRequest._method;
	line >> _parsedRequest._location >> _parsedRequest._httpVersion;

	pos += 2;
	pre = pos;
	pos = _headerBuf.find("\r\n", pos);
}

void Request::testPrintRequest(void)
{
	std::cout << "Method: " << _parsedRequest._method << std::endl;
	std::cout << "Location: " << _parsedRequest._location << std::endl;
	std::cout << "HTTPVersion: " << _parsedRequest._httpVersion << std::endl;
	std::cout << "HOST: " << _parsedRequest._host << std::endl;
	std::cout << "Accept: " << _parsedRequest._accept << std::endl;
	std::cout << "Connection: " << _parsedRequest._connection << std::endl;
	std::cout << "ContentType: " << _parsedRequest._contentType << std::endl;
	std::cout << "ContentLength: " << _parsedRequest._contentLength << std::endl;
	std::cout << "Body: " << _parsedRequest._body << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
}

void Request::_checkValidHeader()
{
	Config &conf = Config::getInstance();

	if (_parsedRequest._location.size() > conf.getClientHeadBufferSize())
		throw(_414_URI_TOO_LONG);
	if (_parsedRequest._transferEncoding != "" && _parsedRequest._contentLength != 0)
		throw(_400_BAD_REQUEST);
	if (_parsedRequest._host == "")
		throw(_400_BAD_REQUEST);
	if (_parsedRequest._contentLength > conf.getClientMaxBodySize())
		throw(_413_REQUEST_ENTITY_TOO_LARGE);
	if (_headerBuf.find("Content-Encoding") != std::string::npos)
		throw(_415_UNSUPPORTED_MEDIA_TYPE);
	if (_parsedRequest._transferEncoding.empty() == false && _parsedRequest._transferEncoding != "chunked")
		throw(_400_BAD_REQUEST);
	// Check Method
	if (_parsedRequest._method != "GET" && _parsedRequest._method != "POST" && _parsedRequest._method != "DELETE")
		throw(_501_NOT_IMPLEMENTED);
	// Check HTTP Version
	if (_parsedRequest._httpVersion.compare(0, 5, "HTTP/") != 0 || _parsedRequest._httpVersion.length() != 8)
		throw(_400_BAD_REQUEST);

	char *endPtr;
	std::string versionStr = _parsedRequest._httpVersion.substr(5);
	double version;

	version = std::strtod(versionStr.c_str(), &endPtr);
	if (*endPtr != '\0')
		throw(_400_BAD_REQUEST);
	if (version > 1.1 || version < 1.0)
		throw(_505_HTTP_VERSION_NOT_SUPPORTED);
	// Host grammar is not correct
	for (size_t i = 0; i < _parsedRequest._host.length(); i++)
	{
		char c = _parsedRequest._host[i];
		if (!std::isalnum(c) && c != '-' && c != '.' && c != ':')
			throw(_400_BAD_REQUEST);
	}
}
