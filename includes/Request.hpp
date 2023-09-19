#pragma once

#include <string>
#include <sstream>
#include <iostream>

#include "Status.hpp"

typedef struct s_request
{
	std::string _method;
	std::string _location;
	std::string _httpVersion;
	std::string _host;
	std::string _accept;
	std::string _connection;
	std::string _contentType;
	unsigned int _contentLength = 0;
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
	void setRequestBufs(const std::string buf);
	t_request getParsedRequest() const;

private:
	std::string _requestBuf;
	t_request _parsedRequest;
	eStatus _statusCode;
	bool _isBody;
};
