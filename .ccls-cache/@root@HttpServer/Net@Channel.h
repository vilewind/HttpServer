/*=============================================================================
#
# Author: vilewind - luochengx2018@163.com
#
# Last modified: 2022-09-05 09:31
#
# Filename: Channel.h
#
# Description: 封装事件类型及对应的回调函数
#
=============================================================================*/
#ifndef __CHANNEL_H__
#define __CAHNEEL_H__

#include "Util.h"
#include <functional>
#include <stdint.h>
#include <atomic>

class EventLoop;

class Channel : public noncopyable
{
public:
	using Callback = std::function<void()>;
	
	Channel(EventLoop*, int);
	~Channel();

	/*< 根据事件调用相应的回调函数 */
	void handleEvent();
	
	void setEvents(uint32_t events) { m_events = events; }
	void setRC(const Callback& cb) { m_readCb = cb; }
	void setWC(const Callback& cb) { m_writeCb = cb; }
	void setEC(const Callback& cb) { m_errorCb = cb; }
	void setInEpoll(bool flag) { m_isInEpoll = flag; }

	int getFd() const { return m_fd; }
	uint32_t getEvents() const { return m_events; }
	bool isInEpoll() { return m_isInEpoll; }

	EventLoop* getOwnerLoop() { return m_ownerLoop; }

private:
	void update();

	EventLoop* m_ownerLoop;
	const int m_fd;
	uint32_t m_events;
	std::atomic<bool> m_isInEpoll;

	Callback m_readCb;
	Callback m_writeCb;
	Callback m_errorCb;
};


#endif
