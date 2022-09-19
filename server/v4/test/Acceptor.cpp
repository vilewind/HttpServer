#include "Acceptor.h"
#include "EventLoop.h"
#include "Channel.h"

Acceptor::Acceptor(EventLoop* loop)
    : loop(loop),
      soc(new SocketUtil::Socket(SocketUtil::Socket::createNoblockSocket())),
      addr(new SocketUtil::Addr("127.0.0.1", 8888)),
      ch(new Channel(loop, soc->getFd()))
{
    std::cout << __func__ << " fd is " << soc->getFd() << std::endl;
    soc->setReusePort();
    soc->bind(*addr);
    soc->listen();
}

Acceptor::~Acceptor() {
    delete soc;
    delete addr;
    delete ch;
    std::cout << __func__ << std::endl;
}

void Acceptor::updateChannelInAceptor()
{
    ch->setReadCb(std::bind(&Acceptor::acceptNewConnection, this));
    ch->setErrorCb(errorCb);
    ch->enableReading();
}

void Acceptor::acceptNewConnection() {
    SocketUtil::Addr addr;
    int fd = soc->accept(addr);
    SocketUtil::Socket::setNonblock(fd);
    connCb(fd);
}

void Acceptor::close()
{
    SocketUtil::Socket::halfClose(soc->getFd(), SHUT_RDWR);
}
