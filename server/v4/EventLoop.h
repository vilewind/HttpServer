/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 08:13
#
# Filename: EventLoop.h
#
# Description: 实现one eventloop per thread
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
#include <thread>
#include <iostream>
#include <mutex>
#include <functional>

class EventLoop : Util::noncopyable
{
public:
	using ChLs = std::vector<Channel*>;
	using Task = std::function<void()>;
	using TaskLs = std::vector<Task>;

	EventLoop();
	~EventLoop();

	void loop();

	void updateChannel(Channel*);

	void removeChannel(Channel*);

	static EventLoop* getEventLoopInCurrentThread();

	bool isInCurrentThread() const { return std::this_thread::get_id() == m_tid; }

	void assertInCurrentThread() {
		if (!isInCurrentThread()) {
			std::cerr << "the owner eventloop do not belong to current thread";
			::exit(EXIT_FAILURE);
		}
	}

	void runInLoop( Task&& );

	void queueInLoop( Task&& );

	void quit();
private:
	/** @brief 通过向eventloop中的eventfd发送数据，防止eventloop阻塞在epoll_wait
	 * */
	void wakeup();

	void wokeup();

	void doPendingTask();

	std::thread::id m_tid;
	std::unique_ptr<Epoller> m_epoller;
	std::atomic<bool> m_looping;
	std::atomic<bool> m_stop;
	int m_efd;
	std::unique_ptr<Channel> m_waker;
	std::mutex m_mtx;
	std::atomic<bool> m_doingPendingTask;

	ChLs m_chs;
	TaskLs m_tasks;
};

#endif
