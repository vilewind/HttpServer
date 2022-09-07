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
#include <sys/epoll.h>

Epoller::Epoller(EventLoop* loop)
	: m_ownerLoop(loop)
{}

Epoller::~Epoller() {}

void Epoller::epoll(Channels& chs, int timeout) {
	chs.clear();
	int num = epoll_wait(

}
