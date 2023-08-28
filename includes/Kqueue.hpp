#ifndef KQUEUE_HPP
#define KQUEUE_HPP

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
    CLIENT
};

class Kqueue
{
public:
    Kqueue();
    Kqueue(const Kqueue &copy);
    Kqueue &operator=(const Kqueue &copy);
    ~Kqueue();
    void addEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
    void enableEvent(const uintptr_t socket);
    void disableEvent(const uintptr_t socket);
    void deleteEvent(const uintptr_t socket);
    int &getKq();
    std::vector<struct kevent> &getEventList();
    std::vector<struct kevent> &getChangeList();
    int doKevent();

public:
    void setFdset(const uintptr_t socket, eFdType flag);
    eFdType getFdType(const uintptr_t socket);

private:
    fd_set _servers;
    fd_set _clients;

private:
    int _kq;
    std::vector<struct kevent> _eventList;
    std::vector<struct kevent> _changeList;
};
#endif
