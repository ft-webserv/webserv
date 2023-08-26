#ifndef Exception_HPP
#define EXception_HPP

#include <exception>
#include <string>

class Exception
{
public:
	Exception();
	Exception(const Exception &copy);
	Exception &operator=(const Exception &copy);
	~Exception();
	static void socketError(const std::string &message);
	static void bindError(const std::string &message);
	static void listenError(const std::string &message);
	static void kqueueError(const std::string &message);
};

#endif
