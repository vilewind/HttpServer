/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 08:13
#
# Filename: EventLoop.h
#
# Description: 
#
=============================================================================*/
#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "Util.h"
#include "Epoller.h"
#include "Channel.h"
#include <atomic>
#include <vector>
#include <memory>

class EventLoop : Util::noncopyable
{
public:
	using ChLs = std::vector<Channel*>;

	EventLoop();
	~EventLoop();

	void loop();

	void updateChannel(Channel*);
private:
	std::unique_ptr<Epoller> m_epoller;
	std::atomic<bool> m_looping;
	ChLs m_chs;
};

#endif
