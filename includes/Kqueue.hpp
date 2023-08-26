#ifndef KQUEUE_HPP
#define KQUEUE_HPP

#include <vector>

// kqueue(), kevent() header
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

class Kqueue
{
public:
    Kqueue();
    Kqueue(const Kqueue& copy);
    Kqueue& operator=(const Kqueue& copy);
    ~Kqueue();

private:
    int kq;
    std::vector<struct kevent> eventList;
    std::vector<struct kevent> changeList;
};
#endif
