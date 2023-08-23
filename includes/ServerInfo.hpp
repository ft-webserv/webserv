#ifndef ServerInfo_HPP
#define ServerInfo_HPP

#include <iostream>
#include <map>
#include <string>

class ServerInfo
{
public:
	ServerInfo(void);
	ServerInfo(std::pair<std::string, std::string> pair);
	ServerInfo(const ServerInfo &src);
	ServerInfo &operator=(ServerInfo const &rhs);
	virtual ~ServerInfo(void);
	void setServerInfo(std::pair<std::string, std::string> pair);
	void getServerInfo();

private:
	std::map<std::string, std::string> _serverInfo;
};

#endif
