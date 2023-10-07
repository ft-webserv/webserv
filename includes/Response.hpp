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

typedef struct s_cgi
{
	std::string cgiExec;
	std::string cgiPath;
	char **env;
	int envCnt;
	int input[2];
	int output[2];
} t_cgi;

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
	bool isCgi();
	std::string &getResponse(); // 굳이 client에서 response를 보내야하는가 ? 그냥 response에서 보내면 되지 않나 ?
	std::string getErrorPage();

private:
	std::string _findRoot();
	void _getFile(std::string root, std::string location);
	void _postFile(std::string root, std::string location, Request &rqs);
	void _deleteFile(std::string root, std::string location);
	std::string _makePath(std::string root, std::string location);
	void _setResponse(std::string path, off_t size);
	void _setBody(std::string path, off_t size);
	void _showFileList(std::string path);
	bool _isAutoIndex();
	void _makeResponse();
	void _createFile(std::string path, std::string location, Request &rqs);
	std::string _makeRandomName();
	void _makeEnvList(uintptr_t clntSock, Request &rqs, std::string root);
	void _addEnv(std::string key, std::string value);
	void _cgiStart();

private:
	eStatus _statusCode;
	std::map<std::string, std::string> _headerFields;
	std::string _body;
	std::string _response;
	std::string _statusText[2][13];
	t_cgi _cgi;
	ServerInfo *_serverInfo;
	LocationInfo *_locationInfo;
};
