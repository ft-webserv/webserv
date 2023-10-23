#include "Logger.hpp"

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::clientConnect(uintptr_t clntSock)
{
	std::cout << BLUE << "[" << printDate() << "]"
			  << "\tClient (" << clntSock << ") Connected" << CLEAR << std::endl;
}

void Logger::clientDisconnect(uintptr_t clntSock)
{
	std::cout << RED
			  << "[" << printDate() << "]"
			  << "\tClient (" << clntSock << ") Disconnected" << CLEAR << std::endl;
}

void Logger::serverRecvFromClient(uintptr_t clntSock)
{
	std::cout << GREEN << "[" << printDate() << "]"
			  << "\tServer Has Recived Data From Client (" << clntSock << ")" << CLEAR << std::endl;
}

void Logger::serverWriteToCgi(pid_t cgiFd)
{
	std::cout << MAGENTA << "[" << printDate() << "]"
			  << "\tServer Has Written Data to CGI (" << cgiFd << ")" << CLEAR << std::endl;
}

void Logger::serverReadFromCgi(pid_t cgiFd)
{
	std::cout << CYAN << "[" << printDate() << "]"
			  << "\tServer Has Read Data From CGI (" << cgiFd << ")" << CLEAR << std::endl;
}

void Logger::serverSendToClient(uintptr_t clntSock)
{
	std::cout << YELLOW << "[" << printDate() << "]"
			  << "\tServer Has Sent Data to Client (" << clntSock << ")" << CLEAR << std::endl;
}
