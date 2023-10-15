#include "Exception.hpp"

Exception::Exception()
{
}

Exception::~Exception()
{
}

void Exception::fileOpenError(const std::string &message)
{
	throw std::runtime_error(message);
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

void Exception::keventError(const std::string &message)
{
	throw std::runtime_error(message);
}

void Exception::acceptError(const std::string &message)
{
	throw std::runtime_error(message);
}

void Exception::recvError(const std::string &message)
{
	throw std::runtime_error(message);
}

void Exception::disconnectDuringRecvError(const std::string &message)
{
	throw std::runtime_error(message);
}

void Exception::disconnectDuringSendError(const std::string &message)
{
	throw std::runtime_error(message);
}

void Exception::chdirError(const std::string &message)
{
	throw std::runtime_error(message);
}
