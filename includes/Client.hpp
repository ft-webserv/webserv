#pragma once

#include <sys/socket.h>

#include "Kqueue.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Exception.hpp"
#include "Error.hpp"
#include "CGI.hpp"

// user define value
#define DEFAULTERRORPAGE "./error_page/html"

enum eClientStatus
{
  START,
  READHEADER,
  READBODY,
  READCHUNKEDBODY,
  FINREAD,
  RESPONSE_START,
  WRITTING,
  FINWRITE
};

class Client : public Error
{
public:
  Client(uintptr_t socket);
  Client(const Client &src);
  Client &operator=(const Client &src);
  ~Client();
  void readRequest();
  void writeResponse();

public:
  uintptr_t &getSocket();
  eClientStatus &getClientStatus();
  Request &getRequest();
  Response &getResponse();
  bool getIsCgi();
  void setServerBlock(port_t port);
  void setLocationBlock();
  void setIsCgi();
  void initClient();
  void doCgi();

  // delete keep alive time

private:
  std::string::size_type getNextPos(std::string::size_type currPos);

private:
  eClientStatus _status;
  uintptr_t _socket;
  Request _request;
  Response _response;
};
