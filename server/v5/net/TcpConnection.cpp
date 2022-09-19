/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 10:58
#
# Filename: TcpConnection.cpp
#
# Description: 
#
=============================================================================*/
#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include <unistd.h>
#include <cstring>
#include <cerrno>

static const int BUFSIZE = 2048;

int recvn( int fd, std::string& str );
int sendn( int fd, std::string& str );

TcpConnection::TcpConnection( EventLoop* loop, int fd )
    : m_loop(loop),
      m_fd( fd ),
      m_ch( new Channel( m_loop, m_fd ))
{
    m_ch->setReadCb( std::bind( &TcpConnection::handleRead, this ) );
    m_ch->setWriteCb( std::bind( &TcpConnection::handleWrite, this ) );
    m_ch->setErrorCb( std::bind( &TcpConnection::handleError, this ) );
    m_ch->setCloseCb( std::bind( &TcpConnection::handleClose, this ) );
}

TcpConnection::~TcpConnection() {
    m_loop->removeChannel( m_ch );
    delete m_ch;
    ::close( m_fd );
}

void TcpConnection::addChannelToLoop()
{
    m_ch->enableReading();
    // m_loop->runInLoop([=]()
    // {
    //     m_ch->enableReading();
    // } );
}

void TcpConnection::send( const std::string& str )
{
    std::cout << str << std::endl;
    m_output += str;
    m_loop->runInLoop( std::bind( &TcpConnection::sendInLoop, shared_from_this() ) );
}

void TcpConnection::sendInLoop()
{
    if (m_isDisConn)
    {
        return;
    }

    m_isAsync = false;

    handleWrite();
}

void TcpConnection::shutdown()
{
    m_loop->runInLoop( std::bind( &TcpConnection::shutdownInLoop, shared_from_this() ) );
} 

void TcpConnection::shutdownInLoop()
{
    if (m_isDisConn )
    {
        return;
    }

    m_closeCb( shared_from_this() );                                    //应用层清理连接
    m_loop->runInLoop( std::bind( m_delConnCb, shared_from_this() ) );  //无法自己清理自己，在TcpServer中处理
    m_isDisConn = true;
}

void TcpConnection::handleRead()
{
    //std::cout << __func__ << std::endl;
    int res = recvn( m_fd, m_input );
 std::cout << __func__ << " : " << res << std::endl;
    if ( res > 0 )
    {
        m_msgCb( shared_from_this(),  m_input );            //应用层处理数据
    }
    else if (res < 0 )
    {
        handleError();
    } 
    else 
    {
        handleClose();
    }
}

void TcpConnection::handleWrite()
{
    //std::cout << __func__ << std::endl;
     int res = sendn( m_fd, m_output );
    if ( res > 0 )
    {
        if ( !m_output.empty() )
        {
            m_ch->enableWriting();
        }
        else 
        {
            m_ch->disableWriting();
            if ( m_ishalfClose )
                handleClose();
        }
    }
    else if ( res < 0 )
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
    if ( m_isDisConn )
    {
        return;
    }
    m_errorCb( shared_from_this() );
    m_loop->runInLoop( std::bind( m_delConnCb, shared_from_this() ) );
    m_isDisConn = true;
}

/**
 * @brief socket可以使用close和shutdown关闭连接，服务器应当采取发送完税局在close的方式
*/
void TcpConnection::handleClose()
{
    //std::cout << __func__ << std::endl;
    if ( m_isDisConn )
    {
        return;
    }

    if ( !m_output.empty() | !m_input.empty() | m_isAsync )
    {
    /* 若仍有数据要发送或数据在处理（产生新的待发送数据），则设置半关闭位*/    
        m_ishalfClose = true;
    /* 可能出现接收到数据后，立即接受到FIN标志，仍然要对这部分数据进行处理*/
        if ( !m_input.empty() )
        {
            m_msgCb( shared_from_this(), m_input );
        }
    }
    else 
    {
        m_loop->runInLoop( std::bind( m_delConnCb, shared_from_this() ) );
        m_isDisConn = true;
    }
}

int recvn( int fd, std::string& str )
{
    ssize_t n = 0;
    size_t sum = 0;
    char buf[BUFSIZE];

    while( 1 )
    {
        ::memset(&buf, 0, sizeof buf );
        n = ::read( fd, buf, BUFSIZE );

        if ( n < 0 )
        {
        /* 读缓冲区为空，非阻塞返回*/    
            if ( errno == EAGAIN || errno == EWOULDBLOCK )
            {
                return sum;
            }
            else if ( errno == EINTR )
            {
                continue;
            }
            else 
            {
                std::cerr << __func__ << " with errno : " <<  errno << std::endl;
                return -1;
            }
        } 
        else if ( n == 0)
        {
            return 0;
        }
        else 
        {
            str.append( buf );
            sum += n;
        /* 若读到的数据规模不足BUFSIZE，说明读缓冲区暂时为空，立即返回*/    
            if ( n < BUFSIZE )
            {
                return sum;
            }
        }
    }
}

int sendn( int fd , std::string& str )
{
    ssize_t n = 0;
    size_t sum = 0;
    size_t len = str.size();
    char* buf = const_cast<char*>( str.c_str() );

    while( 1 )
    {
        n = ::write( fd, buf + sum, len - sum );
        if ( n < 0 )
        {
            /* ignore SIGPIPE*/
            if ( errno == EAGAIN || errno == EWOULDBLOCK )
            {
                return sum;
            }
            else if ( errno == EINTR )
            {
                continue;
            }
            else 
            {
                std::cerr << __func__ << " with errno : " <<  errno << std::endl;
                return -1; 
            }
        }
        else if ( n > 0 )
        {
            sum += n;
            if ( sum == len )
            {
                str.clear();
                buf = nullptr;
                return sum;
            }
        }
        else {
            return 0;
        }
    }
}