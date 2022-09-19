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
#include "IOThreadPool.h"
#include <map>
#include <memory>
#include <mutex>
#include <functional>

class EventLoop;
class TcpConnection;
class Channel;

class TcpServer
{
public:
    using TcpConnSPtr = std::shared_ptr<TcpConnection>;
    using Callback = std::function<void(const TcpConnSPtr&)>;
    using MsgCallback = std::function<void(const TcpConnSPtr&, std::string&&)>;


    TcpServer(EventLoop*, const char* ip = "127.0.0.1", const int port = 8888);
    ~TcpServer();

    void start();

    void setNewConnCB(const Callback& cb) { m_newConnCb = cb; }
    void setCloseCB(const Callback& cb) { m_closeCb = cb; }
    void setErrorCB(const Callback& cb) { m_errorCb = cb; }
    void setMsgCB(const MsgCallback& cb) { m_msgCb = cb; }
private:
    EventLoop *m_mainLoop;
    SocketUtil::Socket* m_soc;
    SocketUtil::Addr* m_addr;
    Channel* m_acceptor;
    std::mutex m_mtx;
    std::map<int, std::shared_ptr<TcpConnection>> connections;
	std::vector<EventLoop*> subReactors;
	IOThreadPool* IOTP;

    Callback m_newConnCb;
    Callback m_closeCb;
    Callback m_errorCb;
    MsgCallback m_msgCb;

    void newConnection();

    void removeConnection(const TcpConnSPtr&);

    void connectionError();
};

#endif
