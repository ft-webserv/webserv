#include "Logger.hpp"

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::hellowWorld()
{
	std::cout << "\n====== " << BOLD_BLUE << "WebServer is opened!!" << CLEAR << "======\n\n";
}

void Logger::openedServerInfo(int servSock, int port)
{
	std::cout << "       Socket: " << GREEN << servSock << CLEAR;
	std::cout << " | Port: " << GREEN << port << CLEAR << std::endl;
}

void Logger::clientConnect(uintptr_t clntSock)
{
	printDate();
	std::cout << BLUE << "\tClient (" << clntSock << ") Connected" << CLEAR << std::endl;
}

void Logger::clientDisconnect(uintptr_t clntSock)
{
	printDate();
	std::cout << RED << "\tClient (" << clntSock << ") Disconnected" << CLEAR << std::endl;
}

void Logger::serverRecvFromClient(uintptr_t clntSock)
{
	printDate();
	std::cout << GREEN << "\tServer Has Recived Data From Client (" << clntSock << ")" << CLEAR << std::endl;
}

void Logger::serverWriteToCgi(pid_t cgiFd)
{
	printDate();
	std::cout << MAGENTA << "\tServer Has Written Data to CGI (" << cgiFd << ")" << CLEAR << std::endl;
}

void Logger::serverReadFromCgi(pid_t cgiFd)
{
	printDate();
	std::cout << CYAN << "\tServer Has Read Data From CGI (" << cgiFd << ")" << CLEAR << std::endl;
}

void Logger::serverSendToClient(uintptr_t clntSock)
{
	printDate();
	std::cout << YELLOW << "\tServer Has Sent Data to Client (" << clntSock << ")" << CLEAR << std::endl;
}
