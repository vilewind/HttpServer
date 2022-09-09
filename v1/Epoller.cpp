/*=============================================================================
#
# Author: vilewind - luochengx2018@163.com
#
# Last modified: 2022-09-05 12:00
#
# Filename: Epoller.cpp
#
# Description: 
#
=============================================================================*/
#include "Epoller.h"
#include "Channel.h"
#include <iostream>
#include <sys/epoll.h>
#include <cassert>

Epoller::Epoller(EventLoop* loop)
	: m_ownerLoop(loop)
{
	m_epfd = epoll_create(5);
	assert(m_epfd >= 0);
	m_epollEvents.reserve(1024);
}

Epoller::~Epoller() {}

void Epoller::epoll(Channels& chs, int timeout) {
	chs.clear();
	int num = epoll_wait(m_epfd, &*m_epollEvents.begin(), m_epollEvents.capacity(), timeout);
	if (num < 0) {
		/* log error*/
		std::cerr << "epoll wait error";
		exit(-1);
	} else if (num == 0) {
		/* log trace*/
	} else {
		for (int i = 0; i < num; ++i) {
			struct epoll_event ee = m_epollEvents[i];
			Channel* ch = static_cast<Channel*>(ee.data.ptr);
			assert(ch->isInEpoll());
			chs.push_back(ch);
		}
	}
}


void Epoller::updateChannel(Channel* ch) {
	assertInLoopThread();
	/* log trace*/
	 struct epoll_event ee;
	 ee.data.ptr = static_cast<void*>(ch);
	 ee.events = ch->getEvents();
	 if (!ch->isInEpoll()) {		
		if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, ch->getFd(), &ee) < 0) {
		std::cerr << "epoll add error " << errno;
		exit(-1);
		}
		ch->setInEpoll(true);
	} 
	else {
		/* 已经存在与epoll树中的ch，若事件为-1则删除*/
		if (ch->getEvents() & -1) {
			if (epoll_ctl(m_epfd, EPOLL_CTL_DEL, ch->getFd(), &ee) < 0) {
				std::cerr << "epoll del error";
				exit(-1);
			}
		} else {
			if (epoll_ctl(m_epfd, EPOLL_CTL_MOD, ch->getFd(), &ee) < 0) {
				std::cerr << "epoll mod error";
				exit(-1);
			}
		}
	}
}

