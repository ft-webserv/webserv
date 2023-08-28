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

class Kqueue
{
public:
    Kqueue();
    Kqueue(const Kqueue &copy);
    Kqueue &operator=(const Kqueue &copy);
    ~Kqueue();
    void addEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
    void enableEvent(const int socket);
    void disableEvent(const int socket);
    void deleteEvent(const int socket);
    int &getKq();
    std::vector<struct kevent> &getEventList();
    std::vector<struct kevent> &getChangeList();

private:
    int _kq;
    std::vector<struct kevent> _eventList;
    std::vector<struct kevent> _changeList;
};
#endif
