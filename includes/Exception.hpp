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
	static void unvalidArgumentsError();
	static void fileOpenError();
	static void socketError();
	static void bindError();
	static void listenError();
	static void kqueueError();
	static void keventError();
	static void acceptError();
	static void recvError();
	static void disconnectDuringRecvError();
	static void disconnectDuringSendError();
};
