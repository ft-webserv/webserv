#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>

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
	Request(const Request &src);
	Request &operator=(Request const &rhs);
	~Request();
	void parseRequest();
	void testPrintRequest();

public:
	std::string setHeaderBuf(std::string &buf);
	void setBodyBuf(std::string &buf);
	void setChunkedBodyBuf(const std::string buf);
	const t_request &getParsedRequest() const;
	void initRequest();
	const bool &getIsBody() const;
	std::map<std::string, std::string> &getEtcHeader();

private:
	std::map<std::string, std::string> _etcHeader;
	std::string _headerBuf;
	t_request _parsedRequest;
	bool _isBody;
	void _parseStartLine(std::string::size_type &pos, std::string::size_type &pre);
	void _checkValidHeader();
};
