#pragma once

#include <map>
#include <string>

#include "ServerInfo.hpp"
#include "Request.hpp"

class Response
{
public:
	Response();
	~Response();
	void handleGet(ServerInfo *servInfo, Request *req);
	void handlePost();
	void handleDelete();

private:
	void setHeaderFields();
	void setStatusLine();
	void setResponse();

private:
	std::map<std::string, std::string> headerFields;
	std::string statusLine;
	std::string response;
};
