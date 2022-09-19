/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 10:58
#
# Filename: Connection.cpp
#
# Description: 
#
=============================================================================*/
#include "Connection.h"
#include "Channel.h"
#include "EventLoop.h"
#include <unistd.h>
#include <cstring>
#include <cassert>

/**
 * @brief noblock IO + EPOLLET 循环读，直到出错或读缓冲区为空
*/
int readn( int fd, std::string&& str) 
{
    size_t nr = 0;
    while(1) {
        nr = ::read(fd, &*str.begin(), str.capacity());
        if (nr < 0) {
            if ( errno == EWOULDBLOCK || errno == EAGAIN)
                return;
            else if ( errno == EINTR)
                continue;
            else 
            {
                str.clear();
                return -1;
            }
        }
        else if (nr == 0) 
        {
            str.clear();
            return 0;
        }
    }
    return 1;
}

int writen( int fd, std::string&& str) 
{
    size_t nr = 0;
    size_t remain = str.size();
    char* buf = const_cast<char*>(str.c_str());
    while(remain > 0)
    {
        nr = ::write(fd, buf + nr, remain);
        if (nr < 0)
            return -1;
        remain -= nr;
    }
    
    return 1;
}




Connection::Connection( EventLoop* loop, int fd )
    : loop( loop ),
      css( kConnecting ),
      m_reading( false )
{
    soc = new SocketUtil::Socket(fd);
    ch = new Channel(loop, fd);
    ch->setReadCb(std::bind(Connection::handleRead, this));
    ch->setWriteCb(std::bind(Connection::handleWrite, this));
    ch->setErrorCb(std::bind(Connection::handleError, this));
    ch->setCloseCb(std::bind(Connection::handleClose, this));
    // ch->enableReading();
}

Connection::~Connection() {
    delete soc;
    delete ch;
    assert(css == kConnected);
}

void Connection::sendInLoop( std::string&& str ) 
{
    loop->assertInCurrentThread();
    size_t nw = 0;
    size_t remain = str.size();
    char* buf = const_cast< char* >( str.c_str() );                 //强制去除常量性
    
    /* ET 模式，直到读完停止*/
    while( remain > 0 ) 
    {
        nw = ::write(ch->getFd(), buf + nw, remain);
        /* if ( num < 0 )*/
        remain -= nw;
    }
}

void Connection::send() 
{
    if ( css == kConnected && !output.empty() ) 
    {
        if ( loop->isInCurrentThread() ) 
        {
            sendInLoop( std::move( output ) );
        } else {
            loop->runInLoop( [=](){
                sendInLoop( std::move( output ) );
            } );
        }
    }
}

void Connection::shutdown() 
{
    if ( css == kConnected ) 
    {
        setState(kConnecting);
        loop->runInLoop(std::bind(&Connection::shutdownInLoop, this));
    }
}

void Connection::shutdownInLoop() {
    loop->assertInCurrentThread();
    if (!ch->getIsWriting()) {
        soc->shutdownWrite();
    }
}

void Connection::forceClose() 
{
    if ( css == kConnected || css == kDisconnecting ) 
    {
        setState(kDisconnecting);
        loop->queueInLoop( std::move( std::bind(&Connection::forceCloseInLoop, this) ) );
    }
}

void Connection::forceCloseInLoop() 
{
    loop->assertInCurrentThread();
    if (css == kConnected || css == kDisconnecting)
    {
        handleClose();
    }
}

void Connection::startRead() 
{
    loop->queueInLoop( std::move( std::bind( &Connection::startReadInLoop, this ) ) );
}

void Connection::startReadInLoop() 
{
    loop->assertInCurrentThread();
    if ( !ch->getIsReading() )
        ch->enableReading();
}

void Connection::stopRead() 
{
    loop->queueInLoop( std::move( std::bind( &Connection::stopReadInLoop, this ) ) );
}

void Connection::stopReadInLoop() 
{
    loop->assertInCurrentThread();
    if (ch->getIsReading())
        ch->disableReading();
}

void Connection::connect() 
{
    loop->assertInCurrentThread();
    assert(css == kConnecting);
    setState(kConnected);
    ch->enableReading();

    conn_cb(shared_from_this());
}

void Connection::disconnect() 
{
    loop->assertInCurrentThread();
    if (css == kConnected) 
    {
        setState(kDisconnected);
        ch->disableAll();

        conn_cb(shared_from_this());
    }
    ch->remove();
}

void Connection::handleRead() 
{
    loop->assertInCurrentThread();
    if ( ch->getIsReading()) 
    {
        input.clear();
        int flag = readn(ch->getFd(), std::move(input));
        if (flag == 1)
        {
            msg_cb(shared_from_this(), std::move(input) );
        } 
        else if (flag == 0) 
        {
            handleClose();
        }
        else 
        {
            handleError();
        }
    }
}

void Connection::handleWrite() 
{
    loop->assertInCurrentThread();
    if ( ch->getIsWriting()) 
    {
        int flag = writen(ch->getFd(), std::move(output) );
        if (flag == 1)
        {
            ch->disableWriting();
            if (css == kDisconnecting)
                shutdownInLoop();
        } 
        else 
        {
            std::cerr << "handle write error";
        }
    }
}

void Connection::handleClose()
{
    loop->assertInCurrentThread();
    assert(css == kConnected || css == kDisconnecting);
    setState(kDisconnected);
    ch->disableAll();

    conn_cb(shared_from_this());
    close_cb(shared_from_this());
}

void Connection::handleError() 
{
    int err = SocketUtil::Socket::getSocketError(ch->getFd());
    std::cerr << "the connection with fd = " << ch->getFd() << " handle error : " << err;
}