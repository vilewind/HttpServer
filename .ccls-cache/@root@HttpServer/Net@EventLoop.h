/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-05 07:17
#
# Filename: EventLoop.h
#
# Description: 处理IO事件的对象，保证one loop per thread 
#
=============================================================================*/
#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "Util.h"
#include "TimerWheel.h"
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>

class Channel;
class Epoller;

class EventLoop : public noncopyable
{
public:
	using Channels = std::vector<Channel*>;
	using Functor = std::function<void()>;

	EventLoop();
	~EventLoop();

	void loop();

	void assertInLoopThread() {
		if (!isInLoopThread()) {
			abortNotInLoopThread();
		}
	}

	bool isInLoopThread() const { return m_tid == std::this_thread::get_id(); }

	EventLoop* getEventLoopInCurrentThread();

	void quit();

	void updateChannel(Channel*);

	void addTimer(int timeout, const TimerWheel::Callback& cb, Timer::TimerType timertype = Timer::TimerType::ONCE_TIMER);
/* 执行用户回调或跨线程唤醒线程执行回调*/
	void runInLoop(const Functor& cb);
/* 跨线程执行回调*/
	void queueInLoop(const Functor& cb);
private:
	void abortNotInLoopThread();
/* 唤醒线程 写*/
	void wakeup();
/* 响应唤醒事件 读*/
	void wokeup();

	void doPendingFunc();


	std::atomic<bool> m_looping;
	std::atomic<bool> m_quit;
	const std::thread::id m_tid;
	std::unique_ptr<Epoller> m_epoller;
	std::unique_ptr<TimerWheel> m_timerWheel;
	std::unique_ptr<Channel> m_wakeupChannel;		//用于跨线程唤醒
	std::mutex m_mutex;
	std::vector<Functor> m_pendingFuncs;
	

	Channels m_channelList;
};

#endif
