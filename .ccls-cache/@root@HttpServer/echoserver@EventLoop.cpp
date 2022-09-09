/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-08 11:17
#
# Filename: EventLoop.cpp
#
# Description: 
#
=============================================================================*/
#include "EventLoop.h"
#include "Epoller.h"
#include "Channel.h"
#include <iostream>
#include <cassert>

thread_local EventLoop* t_loopIncurrentThread = nullptr;

EventLoop::EventLoop()
	: m_looping(false),
	  m_tid(std::this_thread::get_id()),
	  m_epoller(std::make_unique<Epoller>(this))
{
	if (t_loopIncurrentThread == nullptr) {
		t_loopIncurrentThread = this;
	} else {
		std::cerr << "there exists another eventloop in current thread";
		exit(EXIT_FAILURE);
	}
}

EventLoop::~EventLoop() {
	assert(!m_looping);
	t_loopIncurrentThread = nullptr;
}

void EventLoop::assertInLoopThread() {
	if (!inThisThread()) {
		std::cerr << "current task does not belong to this loop";
		exit(EXIT_FAILURE); 
	}
}

EventLoop* EventLoop::getEventLoopInCurrentThread() {
	return t_loopIncurrentThread;
}

void EventLoop::loop() {
	assert(!m_looping);
	assertInLoopThread();
	m_looping = true;
	while(m_looping) {
		m_epoller->epollWait(m_chls);
		for (auto ch : m_chls) {
			ch->handleEvent();
		}
	}

	m_looping = false;
}

void EventLoop::updateChannel(Channel* ch) {
	m_epoller->updateChannel(ch);
}
