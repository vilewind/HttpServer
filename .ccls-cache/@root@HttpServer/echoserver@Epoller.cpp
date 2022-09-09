/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-08 11:55
#
# Filename: Epoller.cpp
#
# Description: 
#
=============================================================================*/
#include "Epoller.h"
#include "Channel.h"
#include "EventLoop.h"
#include "../include/Util.h"

#include <sys/epoll.h>
#include <unistd.h>

Epoller::Epoller(EventLoop* loop)
	: loop(loop),
	  efd(-1)
{
	/**
	 * @brief epoll_create与epoll_create1的关联 
	 * @cite https://blog.csdn.net/weixin_43705457/article/details/104522820*/
	ERRIF("epoll_create", 0, epoll_create1, ::EPOLL_CLOEXEC); 
	evls.reserve(1024);
}

Epoller::~Epoller() {
	::close(efd);
}
	
void Epoller::updateChannel(Channel* ch) {
	int fd = ch->getFd();
	epoll_event event;
	event.data.ptr = static_cast<void*>(ch);
	/* 存在epoll内核表中，可以mod和del*/
	if (ch->getIsInLoop()) {
		if (ch->getEvents() & 0) {
			ERRIF("epoll del", 0, epoll_ctl, efd, EPOLL_CTL_DEL, fd, &event); 
		} else {
			ERRIF("epoll mod", 0, epoll_ctl, efd, EPOLL_CTL_MOD, fd, &event);
		}
		ch->setIsInLoop(false);
	} else {
		ERRIF("epoll add", 0, epoll_ctl, efd, EPOLL_CTL_ADD, fd, &event);
	}
}

void Epoller::epollWait(Epoller::ChannelList& chls, int timeout) {
	chls.clear();
	int num = ERRIF("epoll wait", 0, epoll_wait, efd, &*evls.begin(), evls.capacity(), timeout);

	if (num == 0) {
		/* 定时事件*/
	} else {
		for (int i = 0; i < num; ++i) {
			Channel* ch = static_cast<Channel*>(evls[i].data.ptr);
			chls.push_back(ch);
		}
	}
	
}
