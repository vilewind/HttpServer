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

static thread_local EventLoop* t_eventloop = nullptr;

int getEventFd() 
{
    return Util::ERRIF( "create event fd", 0, ::eventfd, 0, EFD_CLOEXEC | EFD_NONBLOCK );
}

EventLoop::EventLoop() 
    : m_tid ( std::this_thread::get_id() ),
      m_epoller( new Epoller( this )),
      m_efd( getEventFd() ),
      m_waker( new Channel( this, m_efd ) )
{
    if ( t_eventloop != nullptr )
    {
        std::cerr << "there exists another eventloop" << std::endl;
        ::exit( EXIT_FAILURE );
    }

    t_eventloop = this;
    m_waker->setReadCb( [=]()
    {
        wokeup();
    });
    m_waker->enableReading();
}

EventLoop::~EventLoop() 
{
    assert(!m_looping);
    m_waker->disableAll();
    m_waker->remove();
    delete m_epoller;
    delete m_waker;
    m_stop = true;
    t_eventloop = nullptr;
}

void EventLoop::loop() {
    assert(!m_looping);
    assertInCurrentThread();

    m_looping = true;
    while( !m_stop ) 
    {
        m_epoller->epollWait( m_chs );
        for ( Channel* ch : m_chs ) 
        {
            ch->handleEvent();
        }

        execPendingTask();
    }

    m_looping = false;
}

void EventLoop::updateChannel(Channel* ch) {
    assertInCurrentThread();
    m_epoller->updateChannel( ch) ;
    std::cout << "add fd " << ch->getFd() << std::endl;
}

void EventLoop::removeChannel( Channel* ch )
{
    assertInCurrentThread();
    m_epoller->removeChannel( ch );
    std::cout << "del fd " << ch->getFd() << std::endl;
}

void EventLoop::quit()
{
    m_stop = true;

    if ( !isInCurrentThread() )
    {
        wakeup();
    }
}

void EventLoop::runInLoop( Task&& task )
{
    if ( isInCurrentThread() )
    {
        task();
    }
    else 
    {
        queueInLoop( std::move( task ) );
    }
}

void EventLoop::queueInLoop( Task&& task )
{
    {
        std::lock_guard<std::mutex> locker( m_mtx );
        m_tasks.emplace_back( task );
    }

    if ( !isInCurrentThread() | m_pending )
    {
        wakeup();
    }
}

EventLoop* EventLoop::getEventLoopInCurrentThread()
{
    return t_eventloop;
}

void EventLoop::wakeup()
{
    int one = 1;
    Util::ERRIF("wakeup", 1, ::write, m_efd, reinterpret_cast<void*>(&one), sizeof one);
}

void EventLoop::wokeup() {
	int one = 1;
	Util::ERRIF("wokeup", 1, ::read, m_efd, reinterpret_cast<void*>(&one), sizeof one);
}

void EventLoop::execPendingTask()
{
    TaskVec tasks;
    m_pending = true;

    {
        std::lock_guard<std::mutex> locker( m_mtx );
        tasks.swap( m_tasks );
    }

    for (Task& task : tasks )
    {
        task();
    }

    m_pending = false;
}