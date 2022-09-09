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

EventLoop::EventLoop() 
    : m_epoller(std::make_unique<Epoller>(this)),
      m_looping(false)
{

}

EventLoop::~EventLoop() {
    assert(!m_looping);
}

void EventLoop::loop() {
    assert(!m_looping);
    m_looping = true;
    while(m_looping) {
        m_epoller->epollWait(m_chs);
        for (Channel* ch : m_chs) {
            ch->handleEvent();
        }
    }
    m_looping = false;
}

void EventLoop::updateChannel(Channel* ch) {
    m_epoller->updateChannel(ch);
    std::cout << "add fd " << ch->getFd() << std::endl;
}
