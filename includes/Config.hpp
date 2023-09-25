#pragma once

#include "ServerInfo.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <exception>
#include <set>
#include <sstream>

#define WORK_PATH "/Users/youngmch/Webserv"
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
	int &getKeepAliveTime();
	int &getRequestTime();
	int &getClientHeadBufferSize();
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
	int _keepAliveTime;
	int _requestTime;
	int _clientHeaderBufferSize;

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
