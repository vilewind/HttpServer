/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 08:49
#
# Filename: Epoller.cpp
#
# Description: 
#
=============================================================================*/
#include "Epoller.h"
#include "EventLoop.h"
#include "Channel.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>

Epoller::Epoller(EventLoop* loop)
    : m_loop(loop),
      m_epfd(Util::ERRIF("create epoll fd", 0, epoll_create1, EPOLL_CLOEXEC))
{
    eels.reserve(1024);
}

Epoller::~Epoller() {
    Util::ERRIF("close epoll fd", 0, ::close, m_epfd);
}

void Epoller::updateChannel(Channel* ch) {
    struct epoll_event ee;
    memset(&ee, 0, sizeof ee);
    ee.events = ch->getEvents();
    ee.data.ptr = static_cast<void*>(ch);
    int fd = ch->getFd();
    if (ch->getInEPoll()) {
        Util::ERRIF("epoll mod", 0, epoll_ctl, m_epfd, EPOLL_CTL_MOD, fd, &ee);
    } else {
        Util::ERRIF("epoll add", 0, epoll_ctl, m_epfd, EPOLL_CTL_ADD, fd, &ee);
        ch->setInEpoll(true);
    }
}

void Epoller::epollWait(ChLs& chs, int timeout) {
    chs.clear();
    int nfds = Util::ERRIF("epoll wait", 0, ::epoll_wait, m_epfd, &*eels.begin(), eels.capacity(), timeout);
    if (nfds == 0) {
        std::cout << "nothing hannped" << std::endl;
    } else {
        for (int i = 0; i < nfds; ++i) {
            epoll_event ee = eels[i];
            chs.push_back(static_cast<Channel*>(ee.data.ptr));
        }
    }
}


