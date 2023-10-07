#include "Kqueue.hpp"
#include <iostream>

Kqueue::Kqueue()
{
	if ((_kq = kqueue()) == -1)
		Exception::kqueueError("kqueue() error!");
	_eventList.resize(EVENTSIZE);
	FD_ZERO(&_servers);
	FD_ZERO(&_clients);
	FD_ZERO(&_cgis);
}

Kqueue::Kqueue(const Kqueue &copy)
{
	(void)copy;
}

Kqueue &Kqueue::operator=(const Kqueue &copy)
{
	(void)copy;
	return (*this);
}

Kqueue::~Kqueue()
{
}

Kqueue &Kqueue::getInstance()
{
	static Kqueue instance;
	return (instance);
}

void Kqueue::addEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent tempEvent;

	EV_SET(&tempEvent, ident, filter, flags, fflags, data, udata);
	_changeList.push_back(tempEvent);
}

void Kqueue::enableEvent(uintptr_t ident, int16_t filter, void *udata)
{
	struct kevent tempEvent;

	EV_SET(&tempEvent, ident, filter, EV_ENABLE, 0, 0, udata);
	_changeList.push_back(tempEvent);
}

void Kqueue::disableEvent(uintptr_t ident, int16_t filter, void *udata)
{
	struct kevent tempEvent;

	EV_SET(&tempEvent, ident, filter, EV_DISABLE, 0, 0, udata);
	_changeList.push_back(tempEvent);
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
	case CGI:
		FD_SET(socket, &_cgis);
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
	else if (FD_ISSET(socket, &_cgis))
		return (CGI);
	else
		return (DEFAULT);
}

void Kqueue::_deleteFdType(const uintptr_t socket)
{
	switch (getFdType(socket))
	{
	case SERVER:
		FD_CLR(socket, &_servers);
		break;
	case CLIENT:
		FD_CLR(socket, &_clients);
		break;
	case CGI:
		FD_CLR(socket, &_cgis);
		break;
	default:
		break;
	}
}
