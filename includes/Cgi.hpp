#pragma once

#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "Kqueue.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Status.hpp"
#include "Error.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

#define ENVMAXSIZE 40

class Client;

class Cgi : public Error
{
public:
	Cgi(Request *request, Response *response, uintptr_t socket, Client *client);
	~Cgi();
	void writeBody();
	void readResponse();
	uintptr_t getClientSock();
	Response *getResponse();
	pid_t getPid();
	Client *getClient();
	bool getIsCgiFin();
	void cgiStart();
	void deleteCgiEvent();

private:
	void _makeEnvList(uintptr_t clntSock);
	void _addEnv(std::string key, std::string value);
	void _sendResponse();

private:
	std::stringstream _body;
	std::string _cgiExec;
	std::string _cgiPath;
	std::string _cgiResponse;
	char **_env;
	int _envCnt;
	int _reqFd[2];
	int _resFd[2];
	bool _isCgiFin;
	pid_t _pid;
	Response *_response;
	Request *_request;
	uintptr_t _clientSock;
	Client *_client;
	t_cgiInfo _cgiInfo;
};
