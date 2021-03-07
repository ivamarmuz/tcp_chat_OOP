#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "EventSelector.h"

class ChatSession;

class ChatServer : public FdHandler{
    EventSelector *the_selector;
    struct item {
        ChatSession *s;
        item *next;
    };
    item *first;
    ChatServer(EventSelector *sel, int fd);
public:
    ~ChatServer();
    static ChatServer *Start(EventSelector *sel, int port);
    void RemoveSession(ChatSession *s);
    void SendAll(const char *msg, ChatSession *except = 0);
private:
    virtual void Handle(bool r, bool w);
};

#endif