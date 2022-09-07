/*=============================================================================
#
# Author: vilewind - luochengx2018@163.com
#
# Last modified: 2022-09-05 11:45
#
# Filename: Epoller.h
#
# Description: 
#
=============================================================================*/
#ifndef __EPOLLER_H__
#define __EPOLLER_H__

#include "Util.h"
#include "EventLoop.h"
#include <vector>

class Channel;
struct epoll_event;

class Epoller : public noncopyable 
{
public:
	using Channels = std::vector<Channel*>;

	Epoller(EventLoop*);
	~Epoller();

	void epoll(Channels&, int timeout=0);

	void updateChannel(Channel*);

	void assertInLoopThread() { m_ownerLoop->assertInLoopThread(); }

private:
	using EventList = std::vector<epoll_event>;
	
	EventLoop* m_ownerLoop;
	int m_epfd;
	EventList m_epollEvents;
};


#endif
