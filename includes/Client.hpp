#pragma once

#include <sys/socket.h>

#include "Request.hpp"
#include "Response.hpp"
#include "Exception.hpp"

// user define value
#define BUFFERSIZE 1024

class Client
{
public:
  Client(uintptr_t socket);
  Client(const Client& src);
  Client& operator=(const Client& src);
  ~Client();
  void readRequest(intptr_t data);

private:
  uintptr_t _socket;
  Request _request;
  Response _response;
};
