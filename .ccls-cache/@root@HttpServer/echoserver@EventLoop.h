/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-08 11:07
#
# Filename: EventLoop.h
#
# Description: 所处的线程为IO线程，处理IO事件，并且保证one eventloop per thread
#
=============================================================================*/

#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "../include/Util.h"
#include <atomic>
#include <thread>
#include <vector>
#include <memory>

class Channel;
class Epoller;

class EventLoop : public noncopyable
{
public:
	using ChannelList = std::vector<Channel*>;
	
	EventLoop();
	~EventLoop();

	void loop();

	bool inThisThread() const { return std::this_thread::get_id() == m_tid; }

	void assertInLoopThread();

	EventLoop* getEventLoopInCurrentThread();

	void updateChannel(Channel*);
private:
	std::atomic<bool> m_looping;
	std::thread::id m_tid;
	std::unique_ptr<Epoller> m_epoller;

	ChannelList m_chls;
};

#endif

