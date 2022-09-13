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
#include "ThreadPool.h"
#include <map>

class EventLoop;
class Acceptor;
class Connection;

struct Server
{
    EventLoop *mainLoop;
    Acceptor* ac;
    std::map<int, Connection*> connections;
	std::vector<EventLoop*> subReactors;
	ThreadPool<EventLoop> *thP;

    Server(EventLoop*);
    ~Server();

    void acceptor(int);

    void disconnector(int);
};

#endif
