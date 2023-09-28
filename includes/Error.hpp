#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/socket.h>

#include "Config.hpp"
#include "Exception.hpp"
#include "Status.hpp"
#include "Utils.hpp"

class Error
{
public:
	Error();
	~Error();
	void sendErrorPage(uintptr_t socket, std::string errorPagePath, eStatus statusCode);

private:
	std::string _findDefaultPath(eStatus statusCode);
	std::string _findStatusText(eStatus statusCode);
	std::string _defaultPath[2][16];
	std::string _statusText[2][16];
	std::string _response;
};
