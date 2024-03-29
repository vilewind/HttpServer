/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 09:22
#
# Filename: Server.h
#
# Description: 
#
=============================================================================*/
#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "Util.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Channel.h"
#include "IOThreadPool.h"
#include <map>
#include <mutex>

class TcpServer
{
public:
    using TcpConnectionSP = TcpConnection::TcpConnectionSP;
    using MsgCallback = TcpConnection::MsgCallback;
    using ConnCallback = TcpConnection::ConnCallbak;

    TcpServer( EventLoop*, const int threadNum = 0, 
        const char* ip = "127.0.0.1", const uint16_t port = 8888);
    ~TcpServer();

    void start();

    void setAcceptNewConnCb( const ConnCallback& cb ) { m_acceptCb = cb; }

    void setCloseConnCb( const ConnCallback& cb ) { m_closeCb = cb; }

    void setErrorCb( const ConnCallback& cb ) { m_errorCb = cb; }

    void setMsgCb( const MsgCallback& cb ) { m_msgCb = cb; }

private:
/*= func*/
    void acceptNewConn();

    void delConn( const TcpConnectionSP& );

    void dealErrorOnConn();

/*= data*/
    EventLoop* m_loop;
    
    SocketUtil::Addr* m_addr;
    SocketUtil::Socket* m_sock;
    Channel* m_acceptor;

    IOThreadPool* m_ioPool;

    int m_connCount { 0 };
    std::map<int, TcpConnectionSP> m_tcpConns;
    std::mutex m_mtx { };

    ConnCallback m_acceptCb;
    ConnCallback m_closeCb;
    ConnCallback m_errorCb;
    MsgCallback m_msgCb;
};
#endif
