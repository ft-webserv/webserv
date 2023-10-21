#include "Session.hpp"

Session::Session(const std::string newData)
  : _sessionId(_generateSessionId(16)), _sessionData(newData), _lastAccessTime(clock()) {}

Session::Session(const Session &copy)
{
  *this = copy;
}

Session& Session::operator=(const Session &copy)
{
  if (this != &copy)
  {
    this->_sessionId = copy._sessionId;
    this->_sessionData = copy._sessionData;
    this->_lastAccessTime = copy._lastAccessTime;
  }
  return (*this);
}

Session::~Session() {}

const std::string& Session::getSessionId() const { return (_sessionId); }

const std::string& Session::getSessionData() const { return (_sessionData); }

const clock_t& Session::getLastAccessTime() const { return (_lastAccessTime); }

Session& Session::updateAccessTime()
{
  this->_lastAccessTime = clock();
  return (*this);
}

std::string Session::_generateSessionId(int length)
{
  const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  const int charsCount = characters.length();

  std::srand(std::time(0));
  std::string sessionId;

  for (int i = 0; i < length; i++)
  {
      int randomIndex = std::rand() % charsCount;
      sessionId += characters[randomIndex];
  }

  return (sessionId);
}
