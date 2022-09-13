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
#include <functional>

class EventLoop;
class Channel;

using ConnectionCallback = std::function<void(int)>;

struct Connection
{
    EventLoop* loop;
    SocketUtil::Socket* soc;
    Channel* ch;
    ConnectionCallback processConnCb;
    ConnectionCallback delConnCb;                          //断开链接的处理函数

    Connection(EventLoop* loop, int fd);
    ~Connection();

    void echo(int);

    //char read_buf[];


};

#endif
