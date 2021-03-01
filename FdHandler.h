#ifndef FDHANDLER_H
#define FDHANDLER_H

class FdHandler {
    int fd;
public:
    FdHandler(int a_fd): fd(a_fd){}
    virtual ~FdHandler();
    virtual void Handle (bool r, bool w) = 0;
    int GetFd() const { return fd; }
    virtual bool WantRead() const { return true; }
    virtual bool WantWrite() const { return false; }
};

#endif