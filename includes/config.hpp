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
// 싱글톤 보류

class Config
{
public:
	Config();
	Config(const std::string &fileName);
	Config(const Config &source);
	~Config();
	std::map<std::string, std::string> &getgeneralInfo(void);
	std::vector<ServerInfo *> &getServerInfos(void);
	std::set<int> &getPorts(void);
	class FileOpenFailException : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};

private:
	std::map<std::string, std::string> _generalInfo;
	std::vector<ServerInfo *> _serverInfos;
	std::set<int> _ports;

	void parseConfigFile(const std::string &fileName);
	void parseLocationInfo(std::ifstream &file, ServerInfo &target);
	void parseServerInfo(std::ifstream &file);
};
