/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 05:22
#
# Filename: EchoServer.cpp
#
# Description: 
#
=============================================================================*/
#include "EchoServer.h"
#include <sys/epoll.h>
#include <strings.h>
#include <cerrno>

Server::Server()
    : m_soc(createNoblockSocket()),
      m_addr("127.0.0.1", 88),
      m_epfd(ERRIF("epoll create", 0, epoll_create1, 0))
{
    m_soc.bind(m_addr);
    m_soc.listen();
    struct epoll_event event;
    event.data.fd = m_soc.getFd();
    event.events = EPOLLIN | EPOLLET;
    epollAdd(m_soc.getFd(), event);
    m_eventLs.reserve(1024);
    std::cout << "server fd is " << m_soc.getFd() << std::endl;
}

Server::~Server() {
    ::close(m_epfd);
}

void Server::epollAdd(int fd, epoll_event& ee) {
    ERRIF(__func__, 0, epoll_ctl, m_epfd, EPOLL_CTL_ADD, fd, &ee);
    std::cout << "add fd = " << fd << " to epoll" << std::endl;
}

void Server::accept() {
    Addr addr;
    int fd = m_soc.accept(&addr);
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epollAdd(fd, event);
}

void Server::echo(int fd) {
    char buf[1024];
    while(1) {
        bzero(buf, sizeof buf);
        ssize_t n = ::read(fd, buf, sizeof buf);
        if (n == -1) {
            if (errno == EINTR) {
                std::cout << "errno is EINTR; continue" << std::endl;
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cout << "read finished once; break" << std::endl;
                break; 
            }
        } else if (n == 0) {
            std::cout << "peer client disconneted" << std::endl;
            ::close(fd);
            break;
        } else {
            ::write(fd, buf, sizeof buf);
        }
    }
}

void Server::loop() {
    while(1) {
        int num = ERRIF(__func__, 0, epoll_wait, m_epfd, &*m_eventLs.begin(), m_eventLs.capacity(), -1);
        if (num == 0) {
            // std::cout << "nothing happened" << std::endl;
        } else {
            for (auto event : m_eventLs) {
                int fd = event.data.fd;
                if (fd == m_soc.getFd()) {
                    accept();
                } else {
                    if (event.events & EPOLLIN) {
                        echo(fd);
                    } else {
                        // std::cout << "other things happening" << std::endl;
                    }
                }
            }
        }
    
    }
}
