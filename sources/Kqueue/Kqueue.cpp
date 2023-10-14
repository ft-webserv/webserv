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
	// std::cout << "Add Event : " << tempEvent.ident << std::endl;
	_changeList.push_back(tempEvent);
}

void Kqueue::enableEvent(uintptr_t ident, int16_t filter, void *udata)
{
	struct kevent tempEvent;

	EV_SET(&tempEvent, ident, filter, EV_ENABLE, 0, 0, udata);
	_changeList.push_back(tempEvent);
	// std::cout << "enable event ident : " << tempEvent.ident << std::endl;
	// std::cout << "enable event size : " << _changeList.size() << std::endl;
	// std::cout << "enable event filter : " << tempEvent.filter << std::endl;
}

void Kqueue::disableEvent(uintptr_t ident, int16_t filter, void *udata)
{
	struct kevent tempEvent;

	EV_SET(&tempEvent, ident, filter, EV_DISABLE, 0, 0, udata);
	_changeList.push_back(tempEvent);
	// std::cout << "disable event ident : " << tempEvent.ident << std::endl;
	// std::cout << "disable event size : " << _changeList.size() << std::endl;
	// std::cout << "disable event filter : " << tempEvent.filter << std::endl;
}

void Kqueue::deleteEvent(const uintptr_t ident, int16_t filter, void *udata)
{
	struct kevent tempEvent;

	EV_SET(&tempEvent, ident, filter, EV_DELETE, 0, 0, udata);
	// kevent(_kq, &tempEvent, 1, NULL, 0, NULL);
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
	if (res != 0)
	{
		std::cout << "-----------evn list-------------" << std::endl;
		std::cout << "Change List : ";
		for (int i = 0; i < _changeList.size(); i++)
		{
			std::cout << _changeList[i].ident << " ";
		}
		std::cout << std::endl;
		for (int i = 0; i < _changeList.size(); i++)
		{
			std::cout << _changeList[i].filter << " ";
		}
		std::cout << std::endl;
		for (int i = 0; i < _changeList.size(); i++)
		{
			std::cout << _changeList[i].flags << " ";
		}
		std::cout << std::endl;
		std::cout << "Kevent num : " << res << std::endl;
		std::cout << "Event List : ";
		for (int i = 0; i < res; i++)
		{
			std::cout << _eventList[i].ident << " ";
		}
		std::cout << std::endl;
		for (int i = 0; i < res; i++)
		{
			std::cout << _eventList[i].filter << " ";
		}
		std::cout << std::endl;
		std::cout << "-----------evn list-------------" << std::endl;
	}
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
