#include <iostream>

class Logger
{
private:
	Logger();
	~Logger();

public:
	static void clientConnect(uintptr_t clntSock);
	static void clientDisconnect(uintptr_t cgiFd);
	static void serverRecvFromClient(pid_t cgiFd);
	static void serverWriteToCgi(pid_t cgiFd);
	static void serverReadFromCgi(pid_t cgiFd);
	static void serverSendResponseToClient(uintptr_t clntSock);
};
