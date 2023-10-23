#pragma once

#include <exception>
#include <string>

class Exception
{
private:
	Exception();
	~Exception();
<<<<<<< HEAD

public:
=======
	static void unvalidArgumentsError();
>>>>>>> 94314a56d93fffb2e7f8346267b920ff7c666751
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
