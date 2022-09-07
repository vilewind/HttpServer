/*=============================================================================
#
# Author: vilewind - luochengx2018@163.com
#
# Last modified: 2022-09-05 10:08
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
	  m_events(0),
	  m_isInEpoll(false)
{
}

Channel::~Channel() {
}

void Channel::update() {
	m_ownerLoop->updateChannel(this);
}

void Channel::handleEvent() {
	if (m_events & EPOLLIN) {
		if (m_readCb)
			m_readCb();
	}

	if (m_events & EPOLLOUT) {
		if (m_writeCb)
			m_writeCb();
	}

	if (m_events & EPOLLERR) {
		if (m_errorCb)
			m_errorCb();
	}
}

void Channel::enableReading() {
	m_events = EPOLLIN | EPOLLET;
	update();
}


