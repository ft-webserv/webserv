#pragma once

#include <sys/socket.h>

#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Exception.hpp"

// user define value
#define BUFFERSIZE 1024

class Client
{
public:
  Client(uintptr_t socket);
  Client(const Client &src);
  Client &operator=(const Client &src);
  ~Client();
  void readRequest(intptr_t data);
  void writeResponse();
  uintptr_t getSocket();
  void setServerBlock(port_t port);
  void setLocationBlock();

private:
  std::string::size_type getNextPos(std::string::size_type currPos);

private:
  uintptr_t _socket;
  Request _request;
  Response _response;
};
