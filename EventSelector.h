#ifndef EVENTSELECTOR_H
#define EVENTSELECTOR_H

#include "FdHandler.h"

class EventSelector {
    FdHandler **fd_array;
    int fd_array_len;
    int max_fd;
    bool quit_flag;
public:
    EventSelector():fd_array(0), quit_flag(false){}
    ~EventSelector();
    void Add(FdHandler *h);
    bool Remove(FdHandler *h);
    void BreakLoop() { quit_flag = true; }
    void Run();
};

#endif