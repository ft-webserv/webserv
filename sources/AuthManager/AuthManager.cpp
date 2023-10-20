#include "AuthManager.hpp"
#include <utility>

AuthManager::AuthManager() {}

AuthManager::~AuthManager() {}

AuthManager::AuthManager(const AuthManager &copy)
{
  (void)copy;
}

AuthManager& AuthManager::operator=(const AuthManager &copy)
{
  (void)copy;
  return (getInstance());
}

AuthManager& AuthManager::getInstance()
{
  static AuthManager authManager;
  return (authManager);
}

bool AuthManager::authentication(const std::string credentials) const
{
  std::pair<std::string, std::string> idPassword;
  std::string password;

  if (credentials == "")
    return (false);
  idPassword = _decodeBase64(credentials);
  password = _userTable.at(idPassword.first);
  if (password != idPassword.second)
    return (false);
  return (true);
}

std::pair<std::string, std::string> AuthManager::_decodeBase64(const std::string credentials) const
{
  (void)credentials;
  return (std::make_pair("", ""));
}
