#include "Request.hpp"

Request::Request()
	: _isBody(false), _statusCode(_200_OK)
{
	_parsedRequest._contentLength = 0;
}

Request::~Request()
{
}

void Request::setHeaderBuf(const std::string buf)
{
	std::string::size_type newLinePos;

	_headerBuf += buf;
	if (newLinePos = _headerBuf.find("\r\n\r\n") != std::string ::npos)
	{
		_isBody = true;
		_bodyBuf = _headerBuf.substr(newLinePos + 4, _headerBuf.length());
		_headerBuf = _headerBuf.substr(0, newLinePos);
	}
}

void Request::setBodyBuf(const std::string buf)
{
	_bodyBuf += buf;
}

void Request::setChunkedBodyBuf(const std::string buf)
{
	Config &conf = Config::getInstance();

	if (_bodyBuf.size() > conf.getClientMaxBodySize())
		throw(_413_REQUEST_ENTITY_TOO_LARGE);
	_bodyBuf += buf;
}

t_request Request::getParsedRequest() const
{
	return (_parsedRequest);
}

bool Request::getIsBody()
{
	return (_isBody);
}

void Request::parseRequest(void)
{
	std::string::size_type pos = 0;
	std::string::size_type pre = 0;
	std::string word;

	if (_isBody == true)
	{
		pos = _headerBuf.find("\r\n");
		for (; pos < _headerBuf.length();)
		{
			std::istringstream line(_headerBuf.substr(pre, pos));
			line >> word;
			if (word == "GET" || word == "POST" || word == "DELETE")
			{
				_parsedRequest._method = word;
				line >> _parsedRequest._location >> _parsedRequest._httpVersion;
			}
			else if (word == "Host:")
			{
				if (_parsedRequest._host.empty() != 0)
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
				if (_parsedRequest._contentLength != 0)
					throw(_400_BAD_REQUEST);
				line >> _parsedRequest._contentLength;
			}
			else if (word == "Transfer-Encoding:")
			{
				line >> _parsedRequest._transferEncoding;
			}
			pos += 2;
			pre = pos;
			pos = _headerBuf.find("\r\n", pos);
		}
		_checkValidHeader();
	}
	// testPrintRequest();
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
	if (_parsedRequest._contentLength > conf.getClientMaxBodySize())
		throw(_413_REQUEST_ENTITY_TOO_LARGE);
	if (_headerBuf.find("Content-Encoding") != std::string::npos)
		throw(_415_UNSUPPORTED_MEDIA_TYPE);
}
