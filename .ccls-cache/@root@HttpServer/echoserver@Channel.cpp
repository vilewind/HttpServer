/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-08 11:38
#
# Filename: Channel.cpp
#
# Description: 
#
=============================================================================*/
#include "Channel.h"
#include "EventLoop.h"
#include <sys/epoll.h>

Channel::Channel(EventLoop* loop, int fd)
	: m_ownerLoop(loop), 
	  m_fd(fd),
	  m_isInLoop(false),
	  m_events(0)
{
	
}

void Channel::update() {
	//m_ownerLoop->updateChannel(this);
}

void Channel::enableReading() {
	m_events |= EPOLLIN | EPOLLET;
	update();
}

void Channel::handleEvent() {
	if (m_events & EPOLLIN) {
		if (m_RC) 
			m_RC();
	}
	if (m_events & EPOLLOUT) {
		if (m_WC)
			m_WC();
	}
	if (m_events & EPOLLERR) {
		if (m_ERRC)
			m_ERRC();
	}
}
