/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 09:06
#
# Filename: EventLoop.cpp
#
# Description: 
#
=============================================================================*/
#include "EventLoop.h"
#include <cassert>
#include <sys/eventfd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/epoll.h>


static thread_local EventLoop* t_eventLoopInCurrentThread = nullptr;

int getEventFd() {
	return Util::ERRIF("create event fd", 0, eventfd, 0, EFD_CLOEXEC | EFD_NONBLOCK);
}

void EventLoop::wokeup() {
	int one = 1;
	Util::ERRIF("wokeup", 1, ::read, m_efd, reinterpret_cast<void*>(&one), sizeof one);
}

void EventLoop::wakeup() {
	int one = 1;
	Util::ERRIF("wakeup", 1, ::write, m_efd, reinterpret_cast<void*>(&one), sizeof one);
}

EventLoop::EventLoop() 
    : m_epoller(std::make_unique<Epoller>(this)),
      m_looping(false),
	  m_stop(false),
	  m_efd(getEventFd()),
	  m_waker(std::make_unique<Channel>(this, m_efd)),
	  m_mtx(),
	  m_doingPendingTask(false)
{
	if (t_eventLoopInCurrentThread) {
		std::cerr << "there exists another eventloop";
		::exit(EXIT_FAILURE);
	}
	t_eventLoopInCurrentThread = this;
	m_waker->setReadCb(std::bind(&EventLoop::wokeup, this));
	m_waker->enableReading();

}

EventLoop::~EventLoop() {
    assert(!m_looping);
	m_stop = true;
	::close(m_efd);
}

void EventLoop::quit() {
	m_stop = true;
	if (!isInCurrentThread())
		wakeup();
}

void EventLoop::loop() {
    assert(!m_looping);
    m_looping = true;

    while(!m_stop) {
        m_epoller->epollWait(m_chs);
        for (Channel* ch : m_chs) {
            ch->handleEvent();
        }
		doPendingTask();
    }

    m_looping = false;
}

void EventLoop::updateChannel(Channel* ch) {
    m_epoller->updateChannel(ch);
    std::cout << "add fd " << ch->getFd() << std::endl;
}

void EventLoop::removeChannel( Channel* ch )
{
	m_epoller->removeChannel(ch);
	std::cout << "remove fd " << ch->getFd() << std::endl;
}

EventLoop* EventLoop::getEventLoopInCurrentThread() {
	return t_eventLoopInCurrentThread;
}

void EventLoop::runInLoop(Task&& task) {
	if (isInCurrentThread()) {
		task();
	} else {
		queueInLoop( std::move(task) );
	}
}

void EventLoop::queueInLoop(Task&& task) {
	{
		std::lock_guard<std::mutex> locker(m_mtx);
		m_tasks.push_back(task);
	}

	if (!isInCurrentThread() || m_doingPendingTask) {
		wakeup();
	}
}

void EventLoop::doPendingTask() {
	TaskLs tasks;
	{
		std::lock_guard<std::mutex> locker(m_mtx);
		m_tasks.swap(tasks);
	}
	m_doingPendingTask = true;
	for (Task& task : tasks) {
		task();
	}
	m_doingPendingTask = false;
}
