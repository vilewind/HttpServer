#include "Acceptor.h"
#include "EventLoop.h"
#include "Channel.h"

Acceptor::Acceptor(EventLoop* loop)
    : loop(loop)
{
    soc = new SocketUtil::Socket(SocketUtil::Socket::createNoblockSocket());
    addr = new SocketUtil::Addr("127.0.0.1", 8888);
    soc->bind(*addr);
    soc->listen();
    ch = new Channel(loop, soc->getFd());

    std::function<void()> cb = std::bind(&Acceptor::acceptNewConnection, this);
    ch->setReadCb(cb);
    ch->enableReading();
}

Acceptor::~Acceptor() {
    delete soc;
    delete addr;
    delete ch;
}

void Acceptor::acceptNewConnection() {
    SocketUtil::Addr addr;
    int fd = soc->accept(addr);
    SocketUtil::Socket::setNonblock(fd);
    connCb(fd);
}
