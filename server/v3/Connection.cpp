/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 10:58
#
# Filename: Connection.cpp
#
# Description: 
#
=============================================================================*/
#include "Connection.h"
#include "Channel.h"
#include "EventLoop.h"
#include <unistd.h>
#include <cstring>

Connection::Connection(EventLoop* loop, int fd)
    : loop(loop)
{
    soc = new SocketUtil::Socket(fd);
    ch = new Channel(loop, fd);
    Channel::ChannelCb cb = std::bind(&Connection::echo, this, fd);
    ch->setReadCb(cb);
    ch->enableReading();
}

Connection::~Connection() {
    delete soc;
    delete ch;
}

void Connection::echo(int fd) {
    char buf[1024];
    while(true) {
        memset(buf, 0, sizeof buf);
        ssize_t n = ::read(fd, buf, sizeof buf);

        if (n < 0) {
            if (errno == EINTR) {
                std::cout << "errno eintr" << std::endl;
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // std::cout << "errno eagain | ewouldblock" << std::endl;
                break;
            }
        } else if (n == 0) {
            std::cout << "peer client disconneted" << std::endl;
            Util::ERRIF("close", 0, ::close, fd);
        } else {
            ::write(fd, buf, sizeof buf);
        }
    }
}

