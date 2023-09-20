#include "Request.hpp"

Request::Request()
	: _isBody(false), _statusCode(_200_OK)
{
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

void Request::setHeaderBuf(const std::string buf)
{
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
				line >> _parsedRequest._contentLength;
			}
			pos += 2;
			pre = pos;
			pos = _headerBuf.find("\r\n", pos);
		}
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
