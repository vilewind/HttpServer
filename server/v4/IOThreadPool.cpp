/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-13 13:35
#
# Filename: IOThreadPool.cpp
#
# Description: 
#
=============================================================================*/
#include "IOThreadPool.h"
#include "EventLoop.h"

IOThreadPool::IOThreadPool(int size) {
    try
    {
        for (int i = 0; i < size; ++i) {
            m_ths.emplace_back(new std::thread(std::bind(&IOThreadPool::thFunc, this)));
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

IOThreadPool::~IOThreadPool() 
{
    for ( int i = 0; i < m_ths.size(); ++i ) 
    {   
        if (m_ths[i]->joinable())
            m_ths[i]->join();
        delete m_ths[i];
    }
}

void IOThreadPool::thFunc() {
    static thread_local EventLoop el;
    m_reactors.emplace_back(&el);
    el.loop();
}

EventLoop* IOThreadPool::getEventLoop(int fd) {
    /* 随机调度*/
    int cur = fd % m_reactors.size();
    return m_reactors[cur];
}