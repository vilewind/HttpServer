/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 09:22
#
# Filename: Server.h
#
# Description: 
#
=============================================================================*/
#ifndef __SERVER_H__
#define __SERVER_H__

#include "Util.h"

class EventLoop;

struct Server
{
    EventLoop *loop;
    SocketUtil::Socket* soc;
    SocketUtil::Addr* addr;

    Server(EventLoop*);
    ~Server();

    void accpetor();

    void connector(int);
};

#endif
