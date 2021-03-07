#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "ChatSession.h"
#include "ChatServer.h"

static const char welcome_msg[] = "Welcome to the chat, you are know as ";
static const char entered_msg[] = " has entered the chat";
static const char left_msg[] = " has left the chat";

void ChatSession::Send(const char *msg)
{
    write(GetFd(), msg, strlen(msg));
}

ChatSession::ChatSession(ChatServer *a_master, int fd)
    :FdHandler(fd), buf_used(0), ignoring(false), name(0), the_master(a_master)
{
    Send("Your name please: ");
}

ChatSession::~ChatSession()
{
    if (name)
        delete[] name;
}

void ChatSession::Handle(bool r, bool w)
{
    if (!r)
        return;

    if (buf_used >= (int)sizeof(buffer)) {
        buf_used = 0;
        ignoring = true;
    }

    if (ignoring)
        ReadAndIgnore();
    else
        ReadAndCheck();
}

void ChatSession::ReadAndIgnore()
{
    int rc = read(GetFd(), buffer, sizeof(buffer));

    if (rc < 1) {
        the_master->RemoveSession(this);
        return;
    }

    int i;

    for (i = 0; i < rc; i++) {
        if (buffer[i] == '\n') {
            int rest = rc - i - 1;
            if (rest > 0)
                memmove(buffer, buffer + i + 1, rest);
            buf_used = 0;
            ignoring = 0;
            CheckLines();
        }
    }
}

void ChatSession::ReadAndCheck()
{
    int rc = read(GetFd(), buffer+buf_used, sizeof(buffer)-buf_used);

    if (rc < 1) {
        if (name) {
            int len = strlen(name);
            char *lmsg = new char[len + sizeof(left_msg) + 2];
            sprintf(lmsg, "%s%s\n", name, left_msg);
            the_master->SendAll(lmsg, this);
            delete[] lmsg;
        }
        the_master->RemoveSession(this);
        return;
    }
    buf_used += rc;
    CheckLines();
}

void ChatSession::CheckLines()
{
    if (buf_used < 0)
        return;
    
    int i;
    for (i = 0; i < buf_used; i++) {
        if (buffer[i] == '\n') {
            buffer[i] = 0;
            if (i > 0 && buffer[i-1] == '\r')
                buffer[i-1] = 0;
            ProcessLine(buffer);
            int rest = buf_used - i - 1;
            memmove(buffer, buffer + i + 1, rest);
            buf_used = rest;
            CheckLines();
            return;
        }
    }
}

void ChatSession::ProcessLine(const char *str)
{
    int len = strlen(str);
    
    if (!name) {
        name = new char[len+1];
        strcpy(name, str);
        char *wmsg = new char[len + sizeof(welcome_msg) + 2];
        sprintf(wmsg, "%s%s\n", welcome_msg, name);
        Send(wmsg);
        delete[] wmsg;
        char *emsg = new char[len + sizeof(entered_msg) + 2];
        sprintf(wmsg, "%s%s\n", name, entered_msg);
        the_master->SendAll(emsg, this);
        delete[] emsg;
        return;
    }

    int nl = strlen(name);
    char *msg = new char[nl + len + 5];
    sprintf(msg, "<%s> %s\n", name, str);
    the_master->SendAll(msg);
    delete[] msg;
}