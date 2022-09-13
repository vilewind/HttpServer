/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 08:14
#
# Filename: Epoller.h
#
# Description: 
#
=============================================================================*/
#ifndef __EPOLLER_H__
#define __EPOLLER_H__

#include "Util.h"
#include <vector>

class EventLoop;
class Channel;
struct epoll_event;

class Epoller : Util::noncopyable
{
public:
    using ChLs = std::vector<Channel*>;
    using EELs = std::vector<epoll_event>;

    Epoller(EventLoop*);
    ~Epoller();

    void updateChannel(Channel*);

    void epollWait(ChLs&, int timeout=-1);

protected:
    EELs eels;
private:
    EventLoop* m_loop;
    int m_epfd;
};

#endif
