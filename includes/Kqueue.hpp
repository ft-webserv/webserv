#pragma once

#include <vector>
#include "Exception.hpp"

// kqueue(), kevent() header
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

// user define header

#define EVENTSIZE (10)

enum eFdType
{
    DEFAULT,
    SERVER,
    CLIENT,
    CGI
};

class Kqueue
{
public:
    Kqueue &operator=(const Kqueue &copy);
    void addEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
    void enableEvent(const uintptr_t ident, int16_t filter, void *udata);
    void disableEvent(const uintptr_t ident, int16_t filter, void *udata);
    void deleteEvent(const uintptr_t ident, int16_t filter, void *udata);
    int &getKq();
    std::vector<struct kevent> &getEventList();
    std::vector<struct kevent> &getChangeList();
    int doKevent();

public:
    static Kqueue &getInstance();
    void setFdset(const uintptr_t socket, eFdType flag);
    eFdType getFdType(const uintptr_t socket);
    void _deleteFdType(const uintptr_t socket);

private:
    Kqueue();
    Kqueue(const Kqueue &copy);
    ~Kqueue();

private:
    fd_set _servers;
    fd_set _clients;
    fd_set _cgis;

private:
    int _kq;
    std::vector<struct kevent> _eventList;
    std::vector<struct kevent> _changeList;
};
