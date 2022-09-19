/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-15 03:21
#
# Filename: EchoServer.cpp
#
# Description: 
#
=============================================================================*/
#include "EchoServer.h"

EchoServer::EchoServer(EventLoop* loop)
    : tcpserver_(loop)
{
/* 当函数参数为非const的引用时，无法传入左值*/
    tcpserver_.setNewConnCB (std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setMsgCB(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setCloseCB(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.setErrorCB(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.start();
}

EchoServer::~EchoServer()
{

}

void EchoServer::HandleNewConnection(const spTcpConnection& sptcpconn)
{
    std::cout << "New Connection Come in" << std::endl;
}

void EchoServer::HandleMessage(const spTcpConnection& sptcpconn, std::string &&s)
{
    //std::string msg("reply msg:");
    //msg += s;
    //s.clear();
    //swap优化
    std::string msg;
    msg.swap(s);
    msg.insert(0, "reply msg:");
    sptcpconn->send(std::move(msg));
}

void EchoServer::HandleSendComplete(const spTcpConnection& sptcpconn)
{
    //std::cout << "Message send complete" << std::endl;
}

void EchoServer::HandleClose(const spTcpConnection& sptcpconn)
{
    std::cout << "EchoServer conn close" << std::endl;
}

void EchoServer::HandleError(const spTcpConnection& sptcpconn)
{
    std::cout << "EchoServer error" << std::endl;
}

