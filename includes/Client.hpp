#pragma once

#include <sys/socket.h>

#include "Kqueue.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Exception.hpp"
#include "Error.hpp"
#include "Cgi.hpp"
#include "AuthManager.hpp"

// user define value
#define DEFAULTERRORPAGE "./error_page/html"

enum eClientStatus
{
	START,
	READHEADER,
	READBODY,
	READCHUNKEDBODY,
	FINREAD,
	RESPONSE_START,
	WRITTING,
	CGISTART,
	CGIFIN,
	FINWRITE
};

class Client : public Error
{
public:
	Client(uintptr_t socket);
	Client(const Client &src);
	Client &operator=(const Client &src);
	~Client();
	void readRequest(struct kevent *event);
	void writeResponse();

public:
	uintptr_t &getSocket();
	eClientStatus &getClientStatus();
	Request &getRequest();
	Response &getResponse();
	bool getIsCgi();
	Cgi *getCgi();
	void setServerBlock(port_t port);
	void setLocationBlock();
	void setCgi(Cgi *cgi);
	void initClient();

	// delete keep alive time

private:
	std::string::size_type _getNextPos(std::string::size_type currPos);
	void _sendResponse();

private:
	size_t _lastPos;
	std::string _chunkedBodyBuf;
	eClientStatus _status;
	uintptr_t _socket;
	Request _request;
	Response _response;
	Cgi *_cgi;
};
