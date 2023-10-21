#include "Logger.hpp"

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::clientConnect(uintptr_t clntSock)
{
}

void Logger::clientDisconnect(uintptr_t cgiFd)
{
}

void Logger::serverRecvFromClient(uintptr_t cgiFd)
{
}

void Logger::serverWriteToCgi(uintptr_t cgiFd)
{
}

void Logger::serverReadFromCgi(uintptr_t cgiFd)
{
}

void Logger::serverSendResponseToClient(uintptr_t clntSock)
{
}
