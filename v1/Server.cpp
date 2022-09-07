/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-07 12:08
#
# Filename: Server.cpp
#
# Description: 
#
=============================================================================*/
#include "Server.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include <functional>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <cerrno>

Server::Server(EventLoop* loop)
    : el(loop) 
{
    Socket *serv = new Socket();
    Addr *addr = new Addr("127.0.0.1", 8888);
    serv->bind(*addr);
    serv->listen();
    serv->setNonblock();

    Channel *ch = new Channel(el, serv->getFd());
    std::function<void()> cb = std::bind(&Server::newConnection, this, *serv);
    ch->setRC(cb);
    ch->enableReading();
}

Server::~Server() {
    /* */
}

void Server::handleRead(int fd) {
    char buf[1024];
    while(true) {
        bzero(buf, sizeof buf);
        ssize_t nr = ::read(fd, buf, sizeof buf);
        if (nr > 0) {
            std::cout << nr << " bytes message from client fd:" << fd << std::endl;
            ::write(fd, buf, sizeof buf);
        } 
        
        else if (nr < 0 && errno == EINTR) {
            std::cout << "client normally broken, continue" << std::endl;
            continue;
        } 
        
        else if (nr < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            std::cout << "having gotten all data once" << std::endl;
            break;
        }
        else if (nr == 0) {
            std::cout << "EOF, client break connection" << std::endl;
            ::close(fd);
            break;
        }
    }
}

void Server::newConnection(Socket& sk) {
    Addr addr;
    Socket *cs = new Socket(sk.accept(addr));
    std::cout << "new client whose ip is " << inet_ntoa(addr.addr.sin_addr) << " ip is "
    << ntohs(addr.addr.sin_port) << " and fd is " << cs->getFd() << std::endl;
    cs->setNonblock();

    Channel *ch = new Channel(el, cs->getFd());
    std::function<void()> cb = std::bind(&Server::handleRead, this, cs->getFd());
    ch->setRC(cb);
    ch->enableReading();
}