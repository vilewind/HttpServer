/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-07 12:08
#
# Filename: Server.h
#
# Description: 
#
=============================================================================*/
#ifndef __SERVER_H__
#define __SERVER_H__

class EventLoop;
class Socket;

class Server
{
public:
    Server(EventLoop*);
    ~Server();

    void handleRead(int);
    void newConnection(Socket&);
private:
    EventLoop *el;
};

#endif
