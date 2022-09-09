/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-08 11:30
#
# Filename: Channel.h
#
# Description: 绑定文件描述符和对应的回调函数，并提供处理IO事件的接口
#
=============================================================================*/
#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "../include/Util.h"
#include <functional>
#include <atomic>

class EventLoop;

class Channel : public noncopyable
{
public:
	using ChannelCallback = std::function<void()>;

	Channel(EventLoop*, int);
	~Channel() = default;

	void setFd(int fd) { m_fd = fd; }
	int getFd() const { return m_fd; }

	void setIsInLoop(bool flag) { m_isInLoop = flag; }
	bool getIsInLoop() const { return m_isInLoop; }

	void setEvents(uint32_t events) { m_events = events; }
	uint32_t getEvents() const { return m_events; }

	EventLoop* getOwnerLoop() { return m_ownerLoop; }

	void setReadCallback(ChannelCallback& cb) { m_RC = cb; }
	void setWriteCallback(ChannelCallback& cb) { m_WC = cb; }
	void setErrorCallback(ChannelCallback& cb) { m_ERRC = cb; }

	void enableReading();

	void handleEvent();
private:
	void update();

	EventLoop* m_ownerLoop;
	int m_fd;
	std::atomic<bool> m_isInLoop;					//epoll，根据是否存在内核表中，采取add和mod操作
	uint32_t m_events;

	ChannelCallback m_RC;							//读事件回调
	ChannelCallback m_WC;							//写事件回调
	ChannelCallback m_ERRC;						    //错误事件回调
};

#endif
