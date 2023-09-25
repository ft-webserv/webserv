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
	// void _400badRequest(uintptr_t socket, std::string errorPagePath);
	// void _401unauthorized(uintptr_t socket, std::string errorPagePath);
	// void _403forbidden(uintptr_t socket, std::string errorPagePath);
	// void _404notFound(uintptr_t socket, std::string errorPagePath);
	// void _405methodNotAllowed(uintptr_t socket, std::string errorPagePath);
	// void _406notAcceptable(uintptr_t socket, std::string errorPagePath);
	// void _408requestTimeout(uintptr_t socket, std::string errorPagePath);
	// void _409conflict(uintptr_t socket, std::string errorPagePath);
	// void _413requestEntityTooLarge(uintptr_t socket, std::string errorPagePath);
	// void _414uriTooLong(uintptr_t socket, std::string errorPagePath);
	// void _500internalServerError(uintptr_t socket, std::string errorPagePath);
	// void _501notImplemented(uintptr_t socket, std::string errorPagePath);
	// void _502badGateWay(uintptr_t socket, std::string errorPagePath);
	// void _503serviceUnavailable(uintptr_t socket, std::string errorPagePath);
	// void _504gatewayTimeout(uintptr_t socket, std::string errorPagePath);
	// void _505httpVersionNotSupported(uintptr_t socket, std::string errorPagePath);

private:
	std::string _findDefaultPath(eStatus statusCode);
	std::string _findStatusText(eStatus statusCode);
	std::string _defaultPath[2][16];
	std::string _statusText[2][16];
	std::string _response;
};
