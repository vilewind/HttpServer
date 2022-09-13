/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 10:58
#
# Filename: Acceptor.h
#
# Description: 提供监听功能，注册新连接
#
=============================================================================*/

#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "Util.h"
#include <functional>

class EventLoop;
class Channel;

using ConnectionCallback = std::function<void(int)>;

struct Acceptor
{
    EventLoop* loop;
    SocketUtil::Socket* soc;
    SocketUtil::Addr* addr;
    Channel* ch;
    ConnectionCallback connCb;

    Acceptor(EventLoop*);
    ~Acceptor();

    void acceptNewConnection();
};

#endif