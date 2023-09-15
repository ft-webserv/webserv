#pragma once

#include <ctime>
#include <string>
#include <sstream>
#include <iostream>

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
	void setRequestBufs(const std::string buf);
	t_request getParsedRequest() const;

private:
	std::time_t _startTime;
	std::string _requestBuf;
	t_request _parsedRequest;
	int statusCode;
};