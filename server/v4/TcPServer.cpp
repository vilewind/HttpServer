/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 09:19
#
# Filename: TcpServer.cpp
#
# Description: 
#
=============================================================================*/

#include "EventLoop.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include <functional>
#include <cstring>
#include <unistd.h>
#include <cerrno>

using namespace SocketUtil;

using Callback = std::function<void()>;

TcpServer::TcpServer( EventLoop* loop, const char* ip, const int port )
    : m_mainLoop(loop),
      m_soc(new SocketUtil::Socket(SocketUtil::Socket::createNoblockSocket())),
      m_addr(new SocketUtil::Addr(ip, port))  
{
    std::cout << __func__ << " fd is " << m_soc->getFd() << std::endl;
    m_soc->setReusePort();
    m_soc->bind(*m_addr);
    m_soc->listen();
}

TcpServer::~TcpServer() {
   delete IOTP;
   delete m_soc;
   delete m_addr;
   delete m_acceptor;
   for ( auto it = connections.begin(); it != connections.end(); ) {
		// if ( it->second )
		// {
		// 	delete it->second;
		// }
		/** @bug 直接erase it 会导致it为空，遍历出错*/
	    //connections.erase(it);
		connections.erase(it++);
   }
}

void TcpServer::start()
{
    auto cb = std::bind(&TcpServer::newConnection, this);
    m_acceptor->setReadCb(cb);
    m_acceptor->setErrorCb(std::bind(&TcpServer::connectionError, this));
    m_acceptor->enableReading();
	
	IOTP = new IOThreadPool();
}

void TcpServer::newConnection() 
{
    SocketUtil::Addr addr;
    int cli_fd = m_soc->accept(addr);
    SocketUtil::Socket::setNonblock(cli_fd);
    TcpConnSPtr conn = std::make_shared<TcpConnection>( (IOTP->size() == 0 ? m_mainLoop : IOTP->getEventLoop(cli_fd)), cli_fd );
    Callback cb = std::bind( &TcpServer::removeConnection, this, std::placeholders::_1 );
    conn->setDelConnCb(cb);
    conn->setMsgCb(m_msgCb);
    conn->setClosCb(m_closeCb);
    conn->setErrorCb(m_errorCb);
    {
        std::lock_guard<std::mutex> locker(m_mtx);
        connections[cli_fd] = conn;
    }
    m_newConnCb(conn);
    conn->updateChannelInConnection();
}

void TcpServer::removeConnection(const TcpConnSPtr& tc)
{
    std::lock_guard<std::mutex> locker(m_mtx);
    connections.erase(tc->getFd());
}

void TcpServer::connectionError()
{
    std::cerr << "connection error";
    m_soc->halfClose(m_soc->getFd(), SHUT_RDWR);
}




