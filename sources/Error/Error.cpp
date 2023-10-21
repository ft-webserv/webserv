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
	_defaultPath[0][15] = "./error_page/415.html";
	_defaultPath[1][0] = "./error_page/500.html";
	_defaultPath[1][1] = "./error_page/501.html";
	_defaultPath[1][5] = "./error_page/505.html";

	_statusText[0][0] = "BAD REQUEST";
	_statusText[0][1] = "UNAUTHORIZED";
	_statusText[0][3] = "FORBIDDEN";
	_statusText[0][4] = "NOT FOUND";
	_statusText[0][5] = "METHOD NOT ALLOWED";
	_statusText[0][6] = "NOT ACCEPTABLE";
	_statusText[0][8] = "REQUEST TIMEOUT";
	_statusText[0][13] = "REQUEST ENTITY TOO LARGE";
	_statusText[0][15] = "UNSUPPORTED MEDIA TYPE";

	_statusText[1][0] = "INTERNAL SERVER ERROR";
	_statusText[1][1] = "NOT IMPLEMENTED";
	_statusText[1][5] = "HTTP VERSION NOT SUPPORTED";
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

std::string Error::_findStatusText(eStatus statusCode)
{
	int type = statusCode / 100;
	int detail = statusCode % 100;

	if (type == 4 || type == 5)
	{
		return (_statusText[type - 4][detail]);
	}
	return ("");
}

void Error::sendErrorPage(uintptr_t socket, std::string errorPagePath, eStatus statusCode)
{
	std::string extension;
	std::string::size_type pos;
	std::ifstream errorPage;
	struct stat buf;
	Config &conf = Config::getInstance();

	if (errorPagePath.empty() == true || stat(errorPagePath.c_str(), &buf) == -1)
	{
		errorPagePath = _findDefaultPath(statusCode);
		stat(errorPagePath.c_str(), &buf);
	}
	errorPage.open(errorPagePath);
	if (errorPage.is_open() == false)
		; // Log 만 찍기;
	_response = "HTTP/1.1 " + ft_itos(statusCode) + " " + _findStatusText(statusCode) + "\r\n";
	pos = errorPagePath.rfind(".");
  if (statusCode == _401_UNAUTHORIZED)
    _response += "WWW-Authenticate: Basic realm=admin\r\n";
	if (pos == std::string::npos)
		_response += "Content-Type: application/octet-stream\r\n";
	else
	{
		std::string extension = findExtension(errorPagePath);
		_response += "Content-Type: " + conf.getMimeType().find(extension)->second + "\r\n";
	}
	_response += "Content-Length: " + ft_itos(buf.st_size) + "\r\n\r\n";

	std::string pageBuf;
	pageBuf.resize(buf.st_size);
	errorPage.read(&pageBuf[0], buf.st_size);
	_response += pageBuf;
	send(socket, static_cast<void *>(&_response[0]), _response.size(), 0);
}
