#include <iostream>
#include "Utils.hpp"

#define BLUE "\033[0;94m"
#define RED "\033[0;91m"
#define GREEN "\033[0;92m"
#define MAGENTA "\033[0;95m"
#define CYAN "\033[0;96m"
#define YELLOW "\033[0;93m"
#define CLEAR "\033[0m"

class Logger
{
private:
	Logger();
	~Logger();

public:
	static void clientConnect(uintptr_t clntSock);
	static void clientDisconnect(uintptr_t clntSock);
	static void serverRecvFromClient(uintptr_t clntSock);
	static void serverWriteToCgi(pid_t cgiFd);
	static void serverReadFromCgi(pid_t cgiFd);
	static void serverSendToClient(uintptr_t clntSock);
};
