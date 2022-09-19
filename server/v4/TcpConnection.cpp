/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-15 01:13
#
# Filename: TcpConnection.cpp
#
# Description: 
#
=============================================================================*/
#include "TcpConnection.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>

int readn(int fd, std::string&& str);
int writen(int fd, std::string&& str);

TcpConnection::TcpConnection(EventLoop* loop, int fd)
    : m_loop(loop),
      m_ch(new Channel(m_loop, fd)),
      m_socket(new SocketUtil::Socket(fd)),
      m_inputBuf(),
      m_outputBuf()
{
    // m_ch->setEvents(EPOLLIN | EPOLLET);
    // m_ch->setReadCb(std::bind(&TcpConnection::handleRead, this));
    // m_ch->setWriteCb(std::bind(&TcpConnection::handleWrite, this));
    // m_ch->setErrorCb(std::bind(&TcpConnection::handleError, this));
    // m_ch->setCloseCb(std::bind(&TcpConnection::handleClose, this));
    // m_loop->runInLoop([=]()
    // {
    //     m_ch->enableReading();
    // });
}

TcpConnection::~TcpConnection()
{
    m_loop->removeChannel(m_ch);
    delete m_ch;
    delete m_socket;
    m_inputBuf.clear();
    m_outputBuf.clear();
}

void TcpConnection::updateChannelInConnection()
{
    m_ch->setEvents(EPOLLIN | EPOLLET);
    m_ch->setReadCb(std::bind(&TcpConnection::handleRead, this));
    m_ch->setWriteCb(std::bind(&TcpConnection::handleWrite, this));
    m_ch->setErrorCb(std::bind(&TcpConnection::handleError, this));
    m_ch->setCloseCb(std::bind(&TcpConnection::handleClose, this));
    m_loop->runInLoop([=]()
    {
        m_ch->enableReading();
    });
}

void TcpConnection::send(const std::string&& str)
{
    m_outputBuf += str;
    
    if (m_loop->isInCurrentThread()) 
    {
        sendInLoop();
    }
    else
    {
        m_async = false;        
        m_loop->runInLoop([=]()
        {
            sendInLoop();
        });
    } 
}

void TcpConnection::sendInLoop()
{
    if (m_isDisconn)
        return;
    handleWrite();
}

void TcpConnection::shutdown()
{
    if (m_loop->isInCurrentThread()) 
    {
        shutdownInLoop();
    }
    else 
    {
        m_loop->runInLoop([=]()
        {
            shutdownInLoop();
        });
    }
}

void TcpConnection::shutdownInLoop()
{
    if (m_isDisconn)
        return;
    m_closeCb(shared_from_this());
    m_loop->runInLoop([=]()
    {
        m_delConnCb(shared_from_this());
    });
    m_isDisconn = true;
}

void TcpConnection::handleRead()
{
    int result = readn(m_socket->getFd(), std::move(m_inputBuf));

    if (result > 0)
    {
        m_msgCb(shared_from_this(), std::move(m_inputBuf));
    } 
    else if (result == 0)
    {
        m_isHalfClose = true;
        handleClose();
    }
    else 
    {
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    int result = writen(m_socket->getFd(), std::move(m_outputBuf));
    if (result > 0) 
    {
        /* 输出缓冲区已满，则继续可读*/
        if (!m_outputBuf.empty())
        {
            m_ch->enableWriting();
        }
        else
        {
            m_ch->disableWriting();
            handleClose();
        }
    } 
    else if (result < 0) 
    {
        handleError();
    }
    else 
    {
        handleClose();
    }
}

void TcpConnection::handleError()
{
    if (m_isDisconn)    
        return;
    std::cerr << "tcp connection whose fd is " << m_socket->getFd() << " has a error : " << SocketUtil::Socket::getSocketError(m_socket->getFd());
    m_errorCb(shared_from_this());
    m_loop->runInLoop([=]()
    {
        m_delConnCb(shared_from_this());
    });
    m_isDisconn = true;
}

/**
 * @brief 服务器被动关闭，处理完数据后关闭fd
*/
void TcpConnection::handleClose()
{
    if (m_isDisconn)
        return;
    if (m_isHalfClose)
        SocketUtil::Socket::halfClose(m_socket->getFd(), SHUT_WR);
    if (!m_outputBuf.empty() || !m_inputBuf.empty() || m_async)
    {
        /* 可能出现接收到数据后，立即受到FIN标志的情况*/
        if (!m_inputBuf.empty())
            m_msgCb(shared_from_this(), std::move(m_inputBuf));
    }
    else
    {
        m_loop->runInLoop([=]()
        {
            m_delConnCb(shared_from_this());
        });
        m_isDisconn = true;
    }
}

int readn(int fd, std::string&& str)
{
    ssize_t nr = 0;
    size_t nsum = 0;
    char buf[1024];
    while(1)
    {
        ::memset(buf, 0, 1024);
        nr = ::read(fd, buf, 1024);
        if (nr < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return nsum;
            if (errno == EINTR)
                continue;
            return -1;
        }
        else if (nr == 0)
        {
            /* 对端关闭*/
            return 0;
        }
        
        nsum += nr;
    }
    return nsum;
}

int writen(int fd, std::string&& str)
{
    ssize_t nw = 0;
    size_t nsum = 0;
    size_t reamin = str.size();
    char* buf = nullptr;
    while(1)
    {
        buf =  const_cast<char*>(str.c_str());
        nw = ::write(fd, buf, reamin-nsum);
        if (nw > 0)
        {
            nsum += nw;
            str.erase(0, nw);
            if (str.empty())
                return nsum;
        }
        else if (nw < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return nsum;
            if (errno == EINTR)
                continue;
            return -1;
        }
    }
}