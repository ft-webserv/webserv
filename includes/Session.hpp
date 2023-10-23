#pragma once

#include <string>
#include <cstdlib>
#include <ctime>

class Session
{
public:
	Session(const std::string newData);
	Session(const Session &copy);
	Session &operator=(const Session &copy);
	~Session();
	const std::string &getSessionId() const;
	const std::string &getSessionData() const;
	const clock_t &getLastAccessTime() const;
	Session &updateAccessTime();

private:
	std::string _generateSessionId(int length);

private:
	std::string _sessionId;
	std::string _sessionData;
	clock_t _lastAccessTime;
};
