#include "Error.hpp"

Error::Error()
{
	_defaultPath[0][0] = "./error_page/400.html";
	_defaultPath[0][1] = "./error_page/401.html";
	_defaultPath[0][3] = "./error_page/403.html";
	_defaultPath[0][4] = "./error_page/404.html";
	_defaultPath[0][5] = "./error_page/405.html";
	_defaultPath[0][6] = "./error_page/406.html";
	_defaultPath[0][8] = "./error_page/408.html";
	_defaultPath[0][13] = "./error_page/413.html";
	_defaultPath[1][0] = "./error_page/501.html";
	_defaultPath[1][1] = "./error_page/501.html";
	_defaultPath[1][5] = "./error_page/505.html";
	_statusInfo[0][0] = "BAD REQUEST";
	_statusInfo[0][1] = "UNAUTHORIZED";
	_statusInfo[0][3] = "FORBIDDEN";
	_statusInfo[0][4] = "NOT FOUND";
	_statusInfo[0][5] = "METHOD NOT ALLOWED";
	_statusInfo[0][6] = "NOT ACCEPTABLE";
	_statusInfo[0][8] = "REQUEST TIMEOUT";
	_statusInfo[0][13] = "REQUEST ENTITY TOO LARGE";
	_statusInfo[1][0] = "INTERNAL SERVER ERROR";
	_statusInfo[1][1] = "NOT IMPLEMENTED";
	_statusInfo[1][5] = "HTTP VERSION NOT SUPPORTED";
}

Error::~Error()
{
}

std::string Error::_findDefaultPath(eStatus statusCode)
{
	int type = statusCode / 100;
	int detail = statusCode % 100;

	if (type == 4 || type == 5)
	{
		return (_defaultPath[type - 4][detail]);
	}
	return ("");
}

std::string Error::_findStatusInfo(eStatus statusCode)
{
	int type = statusCode / 100;
	int detail = statusCode % 100;

	if (type == 4 || type == 5)
	{
		return (_statusInfo[type - 4][detail]);
	}
	return ("");
}

void Error::_sendErrorPage(uintptr_t socket, std::string errorPagePath, eStatus statusCode)
{
	std::stringstream ss;
	std::ifstream errorPage;
	struct stat buf;

	if (errorPagePath.empty() == true || stat(errorPagePath.c_str(), &buf) == -1)
	{
		errorPagePath = _findDefaultPath(statusCode);
	}
	errorPage.open(errorPagePath);
	if (errorPage.is_open() == false)
		; // Log 만 찍기;
	ss << statusCode;
	_response = "HTTP/1.1" + ss.str() + _findStatusInfo(statusCode);
	errorPage.read(&(*_response.end()), buf.st_size);
	send(socket, static_cast<void *>(&_response[0]), _response.size(), 0);
}

// void Error::_400badRequest(uintptr_t socket, std::string errorPagePath)
// {

// }

// void Error::_401unauthorized(uintptr_t socket, std::string errorPagePath)
// {
// }

// void Error::_403forbidden(uintptr_t socket, std::string errorPagePath)
// {
// }

// void Error::_404notFound(uintptr_t socket, std::string errorPagePath)
// {
// }

// void Error::_405methodNotAllowed(uintptr_t socket, std::string errorPagePath)
// {
// }

// void Error::_406notAcceptable(uintptr_t socket, std::string errorPagePath)
// {
// }

// void Error::_408requestTimeout(uintptr_t socket, std::string errorPagePath)
// {
// }

// void Error::_409conflict(uintptr_t socket, std::string errorPagePath)
// {
// }

// void Error::_413requestEntityTooLarge(uintptr_t socket, std::string errorPagePath)
// {
// }

// void Error::_414uriTooLong(uintptr_t socket, std::string errorPagePath)
// {
// }

// void Error::_500internalServerError(uintptr_t socket, std::string errorPagePath)
// {
// }

// void Error::_501notImplemented(uintptr_t socket, std::string errorPagePath)
// {
// }
// void Error::_501badGateWay(uintptr_t socket, std::string errorPagePath)
// {
// }

// void Error::_503serviceUnavailable(uintptr_t socket, std::string errorPagePath)
// {
// }

// void Error::_504gatewayTimeout()
// {
// }

// void Error::_505httpVersionNotSupported()
// {
// }