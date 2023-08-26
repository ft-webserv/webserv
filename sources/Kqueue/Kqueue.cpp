#include "Kqueue.hpp"

Kqueue::Kqueue()
{
	if (kq = kqueue() == -1)
	{
		Exception::kqueueError("kqueue() error!");
	}
}

Kqueue::Kqueue(const Kqueue &copy)
{
}

Kqueue &Kqueue::operator=(const Kqueue &copy)
{
}

Kqueue::~Kqueue()
{
}

void Kqueue::addEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent tempEvent;

	EV_SET(&tempEvent, ident, filter, flags, fflags, data, udata);
	_changeList.push_back(tempEvent);
}

void Kqueue::enableEvent(const int Socket)
{
}

void Kqueue::disableEvent(const int Socket)
{
}

void Kqueue::deleteEvent(const int Socket)
{
	close(Socket);
}

std::vector<struct kevent> Kqueue::getEventList()
{
}
