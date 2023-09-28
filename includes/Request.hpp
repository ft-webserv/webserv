#pragma once

#include <string>
#include <sstream>
#include <iostream>

#include "Status.hpp"
#include "Config.hpp"
#include "Utils.hpp"

typedef struct s_request
{
	std::string _method;
	std::string _location;
	std::string _httpVersion;
	std::string _host;
	std::string _accept;
	std::string _connection;
	std::string _contentType;
	std::string _transferEncoding;
	std::string _contentLengthStr;
	size_t _contentLength;
	std::string _body;
} t_request;

class Request
{
public:
	Request();
	~Request();
	void parseRequest();
	void testPrintRequest();

public:
	void setHeaderBuf(const std::string buf);
	void setBodyBuf(const std::string buf);
	void setChunkedBodyBuf(const std::string buf);
	t_request getParsedRequest() const;
	void initRequest();
	bool getIsBody();

private:
	std::string _headerBuf;
	std::string _bodyBuf;
	t_request _parsedRequest;
	bool _isBody;
	void _parseStartLine(std::string::size_type &pos, std::string::size_type &pre);
	void _checkValidHeader();
};
