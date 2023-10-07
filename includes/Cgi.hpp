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
#include "Response.hpp"
#include "Request.hpp"
#include "Status.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

#define ENVMAXSIZE 40

enum eCgiStaus
{
	START
};

class Cgi
{
public:
	Cgi();
	Cgi();
	~Cgi();
	void doCgi();

private:
	std::string _cgiExec;
	std::string _cgiPath;
	char **_env;
	int _envCnt;
	int _input[2];
	int _output[2];
	Response &_response;
	Request &_requset;
};
