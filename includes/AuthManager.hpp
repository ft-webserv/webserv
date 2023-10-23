#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "Session.hpp"

class AuthManager
{
public:
  static AuthManager &getInstance();
  bool authentication(const std::string credentials);
  std::string generateSession(const std::string credentials);
  Session *findSession(const std::string &sessionId);
  void deleteOldSession();

private:
  AuthManager();
  AuthManager(const AuthManager &copy);
  AuthManager &operator=(const AuthManager &copy);
  ~AuthManager();
  std::pair<std::string, std::string> _decodeBase64(const std::string credentials) const;

private:
  std::map<std::string, std::string> _userTable;
  std::vector<Session> _session;
};
