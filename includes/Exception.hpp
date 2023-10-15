#pragma once

#include <exception>
#include <string>

class Exception
{
public:
	Exception();
	Exception(const Exception &copy);
	Exception &operator=(const Exception &copy);
	~Exception();
	static void fileOpenError(const std::string &message);
	static void socketError(const std::string &message);
	static void bindError(const std::string &message);
	static void listenError(const std::string &message);
	static void kqueueError(const std::string &message);
	static void keventError(const std::string &message);
	static void acceptError(const std::string &message);
	static void recvError(const std::string &message);
	static void disconnectDuringRecvError(const std::string &message);
	static void disconnectDuringSendError(const std::string &message);
	static void chdirError(const std::string &message);
};
