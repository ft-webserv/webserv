#include "Request.hpp"

Request::Request()
	: _isBody(false), _statusCode(_200_OK)
{
}

Request::~Request()
{
}

void Request::setRequestBufs(const std::string buf)
{
	_requestBuf = buf;
}

t_request Request::getParsedRequest() const
{
	return (_parsedRequest);
}

void Request::parseRequest(void)
{
	std::string::size_type pos = 0;
	std::string::size_type pre = 0;
	std::string word;

	if (_isBody == false)
	{
		pos = _requestBuf.find("\r\n");
		for (; pos < _requestBuf.length();)
		{
			std::istringstream line(_requestBuf.substr(pre, pos));
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
			pos = _requestBuf.find("\r\n", pos);
			if (_requestBuf.find("\r\n\r\n", pos) != std::string::npos)
			{
				_isBody = true;
				break;
			}
		}
	}
	if (_isBody == true)
	{
		_parsedRequest._body += _requestBuf.substr(pos, _requestBuf.length());
	}
	testPrintRequest();
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
