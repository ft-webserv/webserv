#pragma once

#include <exception>
#include <string>

class Exception
{
private:
	Exception();
	~Exception();

public:
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
