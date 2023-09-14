#pragma once

#include <map>
#include <string>
#include <sys/stat.h>

#include "ServerInfo.hpp"
#include "Request.hpp"
#include "Status.hpp"

class Response
{
public:
	Response();
	~Response();
	void handleGet(ServerInfo *servInfo, Request *req);
	void handlePost();
	void handleDelete();

private:
	void findFile(std::string path);
	void addContentType();

private:
	eStatus statusCode;
	std::map<std::string, std::string> headerFields;
	std::string body;
	std::string response;
};
