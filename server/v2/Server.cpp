/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 09:19
#
# Filename: Server.cpp
#
# Description: 
#
=============================================================================*/

#include "EventLoop.h"
#include "Server.h"
#include <functional>
#include <cstring>
#include <unistd.h>
#include <cerrno>

using namespace SocketUtil;

using Callback = std::function<void()>;

Server::Server(EventLoop* loop)
    : loop(loop)
{
    addr = new Addr("127.0.0.1", 8888);
    soc = new Socket(Socket::createNoblockSocket());

    soc->bind(*addr);
    soc->listen();

    Channel* ch = new Channel(loop, soc->getFd());
    Callback cb = std::bind(&Server::accpetor, this);
    ch->setReadCb(cb);
    ch->enableReading();
}

Server::~Server() {
    delete addr;
    delete soc;
    // delete ch;
}

void Server::accpetor() {
    Addr addr;
    int fd = soc->accept(addr);
    Socket::setNonblock(fd);

    Channel* ch = new Channel(loop, fd);
    Callback cb = std::bind(&Server::connector, this, fd);
    ch->setReadCb(cb);
    ch->enableReading();
}

void Server::connector(int fd) {
    char buf[1024];
    while(true) {
        memset(buf, 0, sizeof buf);
        ssize_t n = ::read(fd, buf, sizeof buf);

        if (n < 0) {
            if (errno == EINTR) {
                std::cout << "errno eintr" << std::endl;
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cout << "errno eagain | ewouldblock" << std::endl;
                break;
            }
        } else if (n == 0) {
            std::cout << "perr client disconneted" << std::endl;
            Util::ERRIF("close", 0, ::close, fd);
        } else {
            ::write(fd, buf, sizeof buf);
        }
    }
}
