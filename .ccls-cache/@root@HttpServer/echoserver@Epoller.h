/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-08 11:48
#
# Filename: Epoller.cpp
#
# Description: 
#
=============================================================================*/
#ifndef __EPOLLER_H__
#define __EPOLLER_H__

#include <vector>
#include "../include/Util.h"

class EventLoop;
class Channel;
struct epoll_event;


struct Epoller : public noncopyable
{
	using ChannelList = std::vector<Channel*>;
	using EventList = std::vector<epoll_event>;
		
	EventLoop* loop;
	int efd;
	EventList evls;

	void epollWait(ChannelList& chlc, int timeout = 0);

	void updateChannel(Channel*);
	
	Epoller(EventLoop*);
	~Epoller();
};

#endif
