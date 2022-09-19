/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 10:54
#
# Filename: Connection.h
#
# Description: 封装TCP connection，包含通信函数和连接管理函数
#
=============================================================================*/
#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "Util.h"
#include <string>
#include <functional>
#include <memory>
#include <atomic>

class EventLoop;
class Channel;


struct Connection : public Util::noncopyable, std::enable_shared_from_this<Connection>
{
    using ConnectionSPtr = std::shared_ptr< Connection >;
    using ConnectionCallback = std::function< void( const ConnectionSPtr& ) >;
    using CloseCallback = std::function< void( const ConnectionSPtr& ) >;
    using MessageCallback = std::function< void( const ConnectionSPtr&, std::string&&) >;
/* 连接状态*/    
    enum ConnectionStatus
    {
        kDisconnected = 0,
        kConnecting, 
        kConnected,
        kDisconnecting
    };

    void handleRead();                              //执行::read并mes_cb()
    void handleWrite();
    void handleError();
    void handleClose();

    void send();
    void sendInLoop(std::string&&);
    
    void shutdown();
    void shutdownInLoop();

    void forceClose();
    void forceCloseInLoop();

    void setState( ConnectionStatus cs ) { css = cs; }
    ConnectionStatus getState() const { return css; }

    void startRead();
    void startReadInLoop();

    void stopRead();
    void stopReadInLoop();

    void connect();
    void disconnect();



    EventLoop* loop;
    SocketUtil::Socket* soc;
    Channel* ch;
    ConnectionStatus css;
    std::atomic<bool> m_reading;

    ConnectionCallback conn_cb;
    CloseCallback close_cb;                         //断开链接的处理函数
    MessageCallback msg_cb;                         //处理read到的数据

    Connection(EventLoop* loop, int fd);
    ~Connection();

/**
 * @brief 读写缓冲
*/
    std::string input;
    int read_idx;                                   //待处理部分
    std::string output;

};

#endif
