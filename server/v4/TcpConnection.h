/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-15 01:13
#
# Filename: TcpConnection.h
#
# Description: 
#
=============================================================================*/
#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "Util.h"
#include "Channel.h"
#include "EventLoop.h"
#include <memory>
#include <functional>
#include <string>
#include <atomic>


class TcpConnection: public std::enable_shared_from_this<TcpConnection>
{
public:
    using TcpConnSPtr = std::shared_ptr<TcpConnection>;
    using Callback = std::function<void(const TcpConnSPtr&)>;
    using MsgCallback = std::function<void(const TcpConnSPtr&, std::string&&)>;

    TcpConnection(EventLoop*, int);
    ~TcpConnection();

    int getFd() const { return m_socket->getFd(); }

    EventLoop* getLoop() const { return m_loop; }

    void updateChannelInConnection();                        //添加所属的channel到epoll中

    void send(const std::string&&);
    void sendInLoop();

    void shutdown();
    void shutdownInLoop();
/* channel对应的回调*/
    void handleRead();
    void handleWrite();
    void handleError();
    void handleClose();

    void setMsgCb(MsgCallback& cb) { m_msgCb = cb; }
    void setClosCb(Callback& cb) { m_closeCb = cb; }
    void setErrorCb(Callback& cb) { m_errorCb = cb; }
    void setDelConnCb(Callback& cb) { m_delConnCb = cb; }

    void setIsAsync(bool flag) { m_async = flag; }

private:
    EventLoop *m_loop;                             
    Channel *m_ch;
    SocketUtil::Socket *m_socket;                            //拥有的Socket对象，负责对fd实现RAII
    std::atomic<bool> m_isHalfClose{false};                  //半关闭标志，对端关闭时，置为true，服务器端关闭写
    std::atomic<bool> m_isDisconn{false};                    //连接断开标志
    std::atomic<bool> m_async{false};                        //异步调用标志，任务交由工作线程池执行时，置为true；异步调用完成置为false

    std::string m_inputBuf;
    size_t m_readIdx {0};                                    //解析inputbuf中的内容使用
    std::string m_outputBuf;


    MsgCallback m_msgCb;
    Callback m_closeCb;
    Callback m_errorCb;
    Callback m_delConnCb;                                   //清理tcp connection对应的回调
};

#endif
