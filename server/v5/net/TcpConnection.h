/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 10:54
#
# Filename: TcpConnection.h
#
# Description: 封装TCP Connection，包含通信函数和连接管理函数
#
=============================================================================*/
#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "Util.h"
#include <functional>
#include <string>
#include <memory>

class EventLoop;
class Channel;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    using TcpConnectionSP = std::shared_ptr<TcpConnection>;
    using ConnCallbak = std::function<void( const TcpConnectionSP& )>;
    using MsgCallback = std::function<void( const TcpConnectionSP&, std::string& )>;

    TcpConnection( EventLoop*, int fd );
    ~TcpConnection();

    int getFd() const { return m_fd; }

    EventLoop* getOwnerLoop() const { return m_loop; }

    void addChannelToLoop();

    void send( const std::string& s);
    void sendInLoop();

    void shutdown();
    void shutdownInLoop();

    void handleRead();
    void handleWrite();
    void handleError();
    void handleClose();

    void setMsgCb( const MsgCallback& cb ) { m_msgCb = cb; }
    void setCloseCb( const ConnCallbak& cb ) { m_closeCb = cb; }
    void setErrorCb( const ConnCallbak& cb ) { m_errorCb = cb ;}
    void setDelConnCb( const ConnCallbak& cb ) { m_delConnCb = cb; }

    void setAsync( bool flag )
    {
        m_isAsync = flag;
    }

private:
    EventLoop* m_loop;                  //当前连接所属的eventloop
    int m_fd;                           //当前连接对应的fd
    Channel* m_ch;                      //当前连接绑定的事件
    bool m_ishalfClose { false };                 //半关闭标志
    bool m_isDisConn { false };                   
    bool m_isAsync { false };                     //是否有工作正在被处理（应用层处理信息）

    std::string m_input { };        
    std::string m_output { };                 

    MsgCallback m_msgCb;
    ConnCallbak m_closeCb;
    ConnCallbak m_errorCb;
    ConnCallbak m_delConnCb;
};

#endif
