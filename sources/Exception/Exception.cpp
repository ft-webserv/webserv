#include "Exception.hpp"

Exception::Exception()
{
}

Exception::~Exception()
{
}

void Exception::fileOpenError()
{
	throw std::runtime_error("file open error!");
}

void Exception::socketError()
{
	throw std::runtime_error("socket() error!");
}

void Exception::bindError()
{
	throw std::runtime_error("bind() error!");
}

void Exception::listenError()
{
	throw std::runtime_error("listen() error!");
}

void Exception::kqueueError()
{
	throw std::runtime_error("kqueue() error!");
}

void Exception::keventError()
{
	throw std::runtime_error("kevent() error!");
}

void Exception::acceptError()
{
	throw std::runtime_error("accept() error!");
}

void Exception::recvError()
{
	throw std::runtime_error("recv() error!");
}

void Exception::disconnectDuringRecvError()
{
	throw std::runtime_error("disconnected during read()");
}

void Exception::disconnectDuringSendError()
{
	throw std::runtime_error("disconnected during send()");
}
