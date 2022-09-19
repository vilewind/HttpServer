/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-15 03:21
#
# Filename: EchoServer.h
#
# Description: 
#
=============================================================================*/
#ifndef __ECHOSERVER_H__
#define __ECHOSERVER_H__

#include <string>
#include "TcpServer.h"
#include "EventLoop.h"
#include "TcpConnection.h"

class EchoServer
{
public:
    typedef std::shared_ptr<TcpConnection> spTcpConnection;
    EchoServer(EventLoop*);
    ~EchoServer();
private:
    void HandleNewConnection(const spTcpConnection& sptcpconn);
    void HandleMessage(const spTcpConnection &sptcpconn, std::string &&s);
    void HandleSendComplete(const spTcpConnection& sptcpconn);
    void HandleClose(const spTcpConnection& sptcpconn);
    void HandleError(const spTcpConnection& sptcpconn);

    TcpServer tcpserver_;
};

#endif
