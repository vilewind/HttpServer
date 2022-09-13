/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 08:15
#
# Filename: Channel.h
#
# Description: 
#
=============================================================================*/
#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "Util.h"
#include <atomic>
#include <functional>

class EventLoop;

class Channel : Util::noncopyable
{
public:
	using ChannelCb = std::function<void()>;
	Channel(EventLoop*, int);
	~Channel();

	void setFd(int fd) { m_fd = fd; }
	int getFd() const { return m_fd; }

	void setEvents(uint32_t events) { m_events = events; }
	uint32_t getEvents() const { return m_events; }

	void setInEpoll(bool flag) { m_inEpoll = flag; }
	bool getInEPoll() const { return m_inEpoll; }

	void setReadCb(const ChannelCb& cb) { m_RC = cb; }
	void setWriteCb(const ChannelCb& cb) { m_WC = cb; }
	
	void enableReading();
	/* 处理fd上绑定的事件*/
	void handleEvent();

private:
	/* 注册或更新事件*/
	void update();

	EventLoop* m_loop;
	int m_fd;
	uint32_t m_events;
	std::atomic<bool> m_inEpoll;								//是否在epoll内核表中

	ChannelCb m_RC;
	ChannelCb m_WC;

};

#endif
