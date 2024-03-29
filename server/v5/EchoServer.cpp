/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-19 06:37
#
# Filename: EchoServer.cpp
#
# Description: 
#
=============================================================================*/
#include "EchoServer.h"
#include <iostream>
#include <functional>

EchoServer::EchoServer( EventLoop* loop, const int threadNum, const char* ip, const uint16_t port)
    : m_tcpServer( new TcpServer( loop, threadNum, ip, port ) )
{
    m_tcpServer->setAcceptNewConnCb( std::bind( &EchoServer::handleNewConn, this, std::placeholders::_1 ) );
    m_tcpServer->setCloseConnCb( std::bind( &EchoServer::handleClose, this, std::placeholders::_1 ) );
    m_tcpServer->setErrorCb( std::bind( &EchoServer::handleError, this, std::placeholders::_1 ) );
    m_tcpServer->setMsgCb( std::bind( &EchoServer::handleMsg, this, std::placeholders::_1, std::placeholders::_2 ) );
}

EchoServer::~EchoServer()
{
    delete m_tcpServer;
    std::cout << __func__ << std::endl;
}

void EchoServer::start()
{
    m_tcpServer->start();
}

void EchoServer::handleNewConn( const TcpConnectionSP& tcsp )
{
    std::cout << __func__ << std::endl;
}

void EchoServer::handleClose( const TcpConnectionSP& tcsp )
{
    std::cout << __func__ << std::endl;
}

void EchoServer::handleError( const TcpConnectionSP& tcsp )
{
    std::cout << __func__ << std::endl;
}

void EchoServer::handleMsg( const TcpConnectionSP& tcsp, std::string& str )
{
    std::string msg;
    msg.swap( str );
    tcsp->send( std::move( msg ) );
}