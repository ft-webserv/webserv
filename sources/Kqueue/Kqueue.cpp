#include "Kqueue.hpp"
#include <iostream>

Kqueue::Kqueue()
{
	if ((_kq = kqueue()) == -1)
	{
		Exception::kqueueError("kqueue() error!");
	}
	_eventList.resize(EVENTSIZE);
	FD_ZERO(&_servers);
	FD_ZERO(&_clients);
}

Kqueue::Kqueue(const Kqueue &copy)
{
}

Kqueue &Kqueue::operator=(const Kqueue &copy)
{
	return (*this);
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

// void Kqueue::enableEvent(uintptr_t ident, int16_t filter)
// {
// 	struct kevent tempEvent;

// 	EV_SET(&tempEvent, ident, filter, flags, fflags, data, udata);
// }

// void Kqueue::disableEvent(uintptr_t ident, int16_t filter)
// {
// 	struct kevent tempEvent;

// 	EV_SET(&tempEvent, ident, filter, flags, fflags, data, udata);
// }

void Kqueue::deleteEvent(const uintptr_t Socket)
{
	close(Socket);
}

int &Kqueue::getKq()
{
	return (_kq);
}

std::vector<struct kevent> &Kqueue::getEventList()
{
	return (_eventList);
}

std::vector<struct kevent> &Kqueue::getChangeList()
{
	return (_changeList);
}

int Kqueue::doKevent()
{
	int res = kevent(_kq, &_changeList[0], _changeList.size(), &_eventList[0], EVENTSIZE, NULL);
	if (res == -1)
		Exception::keventError("kevent() error!");
	_changeList.clear();
	return (res);
}

void Kqueue::setFdset(const uintptr_t socket, eFdType flag)
{
	switch (flag)
	{
	case SERVER:
		FD_SET(socket, &_servers);
		break;
	case CLIENT:
		FD_SET(socket, &_clients);
		break;
	default:
		break;
	}
}

eFdType Kqueue::getFdType(const uintptr_t socket)
{
	if (FD_ISSET(socket, &_servers))
		return (SERVER);
	else if (FD_ISSET(socket, &_clients))
		return (CLIENT);
	else
		return (DEFAULT);
}
