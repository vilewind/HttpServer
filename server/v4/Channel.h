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

	void setREvents(uint32_t events) { m_events = events; }
	uint32_t getEvents() const { return m_events; }

	void setInEpoll(bool flag) { m_inEpoll = flag; }
	bool getInEPoll() const { return m_inEpoll; }

	void setReadCb( const ChannelCb& cb ) { m_RC = cb; }
	void setWriteCb( const ChannelCb& cb ) { m_WC = cb; }
	void setErrorCb( const ChannelCb& cb ) { m_EC = cb; }
	void setCloseCb( const ChannelCb& cb ) { m_CC = cb; }

	void enableReading();
	void disableReading();

	void enableWriting();
	void disableWriting();

	void disableAll();
	/* 处理fd上绑定的事件*/
	void handleEvent();

	/* 移除channel*/
	void remove();

private:
	/* 注册或更新事件*/
	void update();

	EventLoop* m_loop;
	int m_fd;
	uint32_t m_events;
	std::atomic<bool> m_inEpoll { false };								//是否在epoll内核表中

	ChannelCb m_RC;												//read	
	ChannelCb m_WC;												//write	
	ChannelCb m_EC;												//error
	ChannelCb m_CC;												//close

};

#endif
