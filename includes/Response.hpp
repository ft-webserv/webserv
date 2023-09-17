#pragma once

#include <map>
#include <string>
#include <sys/stat.h>

#include "Config.hpp"
#include "Request.hpp"
#include "Status.hpp"

class Response
{
public:
	Response();
	~Response();
	void handleGet(Request &rqs);
	void handlePost(Request &rqs);
	void handleDelete(Request &rqs);
	ServerInfo *getServerInfo();
	LocationInfo *getLocationInfo();
	void setServerInfo(ServerInfo *serverBlock);
	void setLocationInfo(LocationInfo *locationBlock);

private:
	void findFile(std::string path);
	void _addContentType(std::string path);

private:
	eStatus _statusCode;
	std::map<std::string, std::string> _headerFields;
	std::string _body;
	std::string _response;
	ServerInfo *_serverInfo;
	LocationInfo *_locationInfo;
};
