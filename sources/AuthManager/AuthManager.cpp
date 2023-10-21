#include "AuthManager.hpp"

AuthManager::AuthManager()
{
  _userTable["adminId"] = "adminPassword";
  _userTable["siyang"] = "siyang";
}

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

bool AuthManager::authentication(const std::string credentials)
{
  std::pair<std::string, std::string> userData;
  std::map<std::string, std::string>::const_iterator serverData;

  if (credentials == "")
    return (false);

  userData = _decodeBase64(credentials);
  serverData = _userTable.find(userData.first);
  if (serverData == _userTable.end())
    return (false);

  if (serverData->second != userData.second)
    return (false);

  return (true);
}

std::string AuthManager::generateSession(const std::string credentials)
{
  Session newSession(credentials);

  _session.push_back(newSession);
  return ("session-id=" + newSession.getSessionId());
}

Session* AuthManager::findSession(const std::string& sessionId)
{
  std::vector<Session>::iterator it = _session.begin();

  for (; it != _session.end(); it++)
  {
    if (it->getSessionId() == sessionId)
      return (&(*it)); 
  }
  return (NULL);
}

std::pair<std::string, std::string> AuthManager::_decodeBase64(const std::string credentials) const
{
  static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  size_t in_len = credentials.size();
  if (in_len % 4 != 0)
    return (std::make_pair("", ""));

  size_t out_len = in_len / 4 * 3;
  if (credentials[in_len - 1] == '=')
    out_len--;
  if (credentials[in_len - 2] == '=')
    out_len--;

  std::string decoded;
  decoded.resize(out_len);

  for (size_t i = 0, j = 0; i < in_len;) {
      uint32_t sextet_a = credentials[i] == '=' ? 0 & i++ : base64_chars.find(credentials[i++]);
      uint32_t sextet_b = credentials[i] == '=' ? 0 & i++ : base64_chars.find(credentials[i++]);
      uint32_t sextet_c = credentials[i] == '=' ? 0 & i++ : base64_chars.find(credentials[i++]);
      uint32_t sextet_d = credentials[i] == '=' ? 0 & i++ : base64_chars.find(credentials[i++]);

      uint32_t triple = (sextet_a << 18) + (sextet_b << 12) + (sextet_c << 6) + sextet_d;
      if (j < out_len)
        decoded[j++] = (triple >> 16) & 0xFF;
      if (j < out_len) 
        decoded[j++] = (triple >> 8) & 0xFF;
      if (j < out_len)
        decoded[j++] = triple & 0xFF;
  }

  size_t equal = decoded.find(':');
  return (std::make_pair(decoded.substr(0, equal), decoded.substr(equal + 1)));
}
