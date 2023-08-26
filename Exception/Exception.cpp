#include "Exception.hpp"

Exception::Exception()
{
}

Exception::~Exception()
{
}

void Exception::socketError(const std::string &message)
{
	throw std::runtime_error(message);
}

void Exception::bindError(const std::string &message)
{
	throw std::runtime_error(message);
}

void Exception::listenError(const std::string &message)
{
	throw std::runtime_error(message);
}

void Exception::kqueueError(const std::string &message)
{
	throw std::runtime_error(message);
}
