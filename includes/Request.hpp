#pragma once

#include <string>
#include <sstream>
#include <iostream>

#include "Status.hpp"
#include "Config.hpp"

typedef struct s_request
{
	std::string _method;
	std::string _location;
	std::string _httpVersion;
	std::string _host;
	std::string _accept;
	std::string _connection;
	std::string _contentType;
	unsigned int _contentLength;
	std::string _body;
} t_request;

class Request
{
public:
	Request();
	~Request();
	void parseRequest(void);
	void testPrintRequest(void);

public:
	void setHeaderBuf(const std::string buf);
	void setBodyBuf(const std::string buf);
	t_request getParsedRequest() const;
	bool getIsBody();

private:
	std::string _headerBuf;
	std::string _bodyBuf;
	t_request _parsedRequest;
	eStatus _statusCode;
	bool _isBody;
	void _checkValidHeader();
};
