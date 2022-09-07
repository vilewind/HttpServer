/*=============================================================================
#
# Author: vilewind - luochengx2018@163.com
#
# Last modified: 2022-09-05 07:30
#
# Filename: EventLoop.cpp
#
# Description: 
#
=============================================================================*/
#include "EventLoop.h"
#include "Channel.h"
#include "Epoller.h"
#include <cassert>
#include <iostream>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

/**
 * @brief 仿muduo，使用eventfd实现跨线程唤醒
 *
 * */
int createEventFd() {
	int efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (efd < 0) {
		std::cerr << "eventfd error";
		exit(-1);
	}
	return efd;
}


thread_local EventLoop* t_loopInCurrentThread = nullptr;

EventLoop::EventLoop()
	: m_looping(false),
	  m_quit(false),
	  m_tid(std::this_thread::get_id()),
	  m_epoller(new Epoller(this)),
	  m_wakeupChannel(new Channel(this, createEventFd())),
	  m_mutex()

{
	/* log trace*/
	if (t_loopInCurrentThread) {
		/* log fatal*/
	} else {
		t_loopInCurrentThread = this;
	}

	m_wakeupChannel->setEvents(EPOLLIN | EPOLLET);
	m_wakeupChannel->setRC(std::bind(&EventLoop::wokeup, this));

}

EventLoop::~EventLoop() {
	assert(!m_looping);
	t_loopInCurrentThread=nullptr;
}

EventLoop* EventLoop::getEventLoopInCurrentThread() {
	return t_loopInCurrentThread;
}

void EventLoop::loop() {
	assert(!m_looping);
	assertInLoopThread();
	m_looping = true;
	m_quit = false;

	/* loop*/
	while(!m_quit) {
		m_epoller->epoll(m_channelList);
		for (Channel* ch : m_channelList) {
			ch->handleEvent();
		}

		doPendingFunc();
	}

	/* log trace*/
	m_looping = false;
}

void EventLoop::abortNotInLoopThread() {
	/* log fatal*/
	std::cout << " not in loop thread" << std::endl;
	exit(-1);
}

void EventLoop::updateChannel(Channel* ch) {
	assert(ch->getOwnerLoop() == this);
	assertInLoopThread();
	m_epoller->updateChannel(ch);
}

/**
 * @brief 在IO线程中执行某个用户回调，若用户在其他线程中调用runInLoop，
 *			则需要加入对应IO线程中eventloop的任务队列中
 * */
void EventLoop::runInLoop(const Functor& fc) {
	if (isInLoopThread()) {
		fc();
	} else {
		queueInLoop(fc);
	}
}

void EventLoop::queueInLoop(const Functor& fc) {
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_pendingFuncs.push_back(fc);
	}
	wakeup();
}

void EventLoop::doPendingFunc() {
	std::vector<Functor> functors;
	
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		functors.swap(m_pendingFuncs);
	}

	for (Functor fc : functors) {
		fc();
	}

}

void EventLoop::quit() {
	m_quit = true;
	if (!isInLoopThread()) {
		wakeup();
	}
}

void EventLoop::wakeup() {
	uint64_t one = 1;
	ssize_t n = write(m_wakeupChannel->getFd(), &one, sizeof one);
}

void EventLoop::wokeup() {
	uint64_t one = 1;
	ssize_t n = read(m_wakeupChannel->getFd(), &one, sizeof one);
}



























