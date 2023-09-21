#include "Error.hpp"

Error::Error()
{
	memset(_defaultPath, 0, sizeof(std::string) * 2 * 13);
	_defaultPath[0][0] = "";
	_defaultPath[0][1] = "";
	_defaultPath[0][3] = "";
	_defaultPath[0][4] = "";
	_defaultPath[0][5] = "";
	_defaultPath[0][6] = "";
	_defaultPath[0][8] = "";
	_defaultPath[0][13] = "";
	_defaultPath[1][0] = "";
	_defaultPath[1][1] = "";
	_defaultPath[1][5] = "";
}

Error::~Error()
{
}

std::string _getDefaultPath(eStatus statusCode)
{
	int type = statusCode / 100;
	int detail = statusCode % 100;

	if (type == 4 || type == 5)
	{
		return (_defaultPath[type - 4][detail]);
	}
	return ("");
}

void _sendErrorPage(uintptr_t socket, std::string errorPagePath, eStatus statusCode);
{
	std::ifstream errorPage;
	struct stat buf;

	if (errorPagePath.empty)
	{
		errorPagePath = _getDefaultPath(statusCode);
	}
	stat(errorPagePath.c_str(), &buf);
	errorPage.open(errorPagePath);
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
