#include <string>
#include <map>
#include <iostream>

class AuthManager
{
public:
  static AuthManager& getInstance();
  bool authentication(const std::string credentials);

private:
  AuthManager();
  AuthManager(const AuthManager &copy);
  AuthManager& operator=(const AuthManager &copy);
  ~AuthManager();
  std::pair<std::string, std::string> _decodeBase64(const std::string credentials) const;
  // void generateSession(const std::string credentials) const;

private:
  std::map<std::string, std::string> _userTable;
  // std::map<std::string, std::string> sessionTable;

};
