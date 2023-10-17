#pragma once

#include <map>
#include <string>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "Config.hpp"
#include "Request.hpp"
#include "Status.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

#define DEFAULT_ROOT "/html"

typedef struct s_cgiInfo
{
	std::string cgiExtension;
	std::string cgiExec;
	std::string cgiPath;
} t_cgiInfo;

class Response
{
public:
	Response();
	~Response();
	void handleGet(Request &rqs);
	void handlePost(Request &rqs);
	void handleDelete(Request &rqs);
	// void handleCgi(Request &rqs, uintptr_t clntSock);
	ServerInfo *getServerInfo();
	LocationInfo *getLocationInfo();
	void setServerInfo(ServerInfo *serverBlock);
	void setLocationInfo(LocationInfo *locationBlock);
	void initResponse();
	bool isAllowedMethod(std::string method);
	bool isCgi(std::string location);
	bool &getIsHead();
	std::string &getResponse(); // 굳이 client에서 response를 보내야하는가 ? 그냥 response에서 보내면 되지 않나 ?
	std::string &getRoot();
	std::string getErrorPage();
	t_cgiInfo &getCgiInfo();
	std::string &getPath(std::string location);
	void setResponse(std::string &response);

private:
	void _findRoot();
	void _getFile(std::string location);
	void _postFile(std::string location, Request &rqs);
	void _deleteFile(std::string location);
	void _makePath(std::string location);
	void _setHeader(std::string path, off_t size);
	void _setBody(std::string path, off_t size);
	void _showFileList(std::string path);
	bool _isAutoIndex();
	void _makeResponse();
	void _createFile(std::string location, Request &rqs);
	std::string _makeRandomName();

private:
	bool _isHead;
	eStatus _statusCode;
	std::map<std::string, std::string> _headerFields;
	std::string _body;
	std::string _root;
	std::string _path;
	std::string _response;
	std::string _statusText[2][13];
	ServerInfo *_serverInfo;
	LocationInfo *_locationInfo;
	t_cgiInfo _cgiInfo;
  std::size_t _clientMaxBodySize;
};
