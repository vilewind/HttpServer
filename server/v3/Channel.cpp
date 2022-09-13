/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 08:22
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
	: m_loop(loop),
	  m_fd(fd),
	  m_events(0),
	  m_inEpoll(false)
{

}

Channel::~Channel() {
	
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
	/* other task*/
}

void Channel::update() {
	m_loop->updateChannel(this);
}
