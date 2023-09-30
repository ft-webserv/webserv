#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <exception>
#include <set>
#include <sstream>

#include "ServerInfo.hpp"
#include "Utils.hpp"

// #define WORK_PATH "/Users/youngmch/Webserv"
#define WORK_PATH "/Users/youngmin/webserv"
#define BUFFERSIZE 1024

class Config
{
public:
	static Config &getInstance(const std::string &fileName);
	static Config &getInstance();
	std::map<std::string, std::string> &getGeneralInfo();
	std::vector<ServerInfo *> &getServerInfos();
	std::set<int> &getPorts();
	std::map<std::string, std::string> &getMimeType();
	size_t &getKeepAliveTime();
	size_t &getRequestTime();
	size_t &getClientHeadBufferSize();
	size_t &getClientMaxBodySize();
	// it will be die
	class FileOpenFailException : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};

private:
	Config();
	Config(const std::string &fileName);
	Config(const Config &source);
	~Config();

private:
	size_t _keepAliveTime;
	size_t _requestTime;
	size_t _clientHeaderBufferSize;
	size_t _clientMaxBodySize;

private:
	std::map<std::string, std::string> _generalInfo;
	std::vector<ServerInfo *> _serverInfos;
	std::set<int> _ports;
	std::map<std::string, std::string> _mimeType;

	void parseConfigFile(const std::string &fileName);
	void parseLocationInfo(std::ifstream &file, ServerInfo &target);
	void parseServerInfo(std::ifstream &file);
	void parseInclude(std::ifstream &file);
};
