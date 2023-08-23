#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerInfo.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <exception>
// 싱글톤 보류

class Config
{
public:
	Config();
	Config(const std::string &fileName);
	Config(const Config &source);
	~Config();
	std::map<std::string, std::string> getGeneralBlock();
	std::vector<ServerInfo *> getSeverBlock();
	class FileOpenFailException : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};

private:
	std::map<std::string, std::string> _generalBlock;
	std::vector<ServerInfo *> _serverInfos;

	void parseConfigFile(const std::string &fileName);
	std::pair<std::string, std::string> getPair(std::ifstream &file, std::string word);
	void parseLocationInfo(std::ifstream &file, ServerInfo &target);
	void parseServerInfo(std::ifstream &file);
};

#endif
