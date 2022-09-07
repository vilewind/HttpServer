/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-07 11:42
#
# Filename: Socket.cpp
#
# Description: 
#
=============================================================================*/
#include "Socket.h"
#include "Util.h"
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>

Addr::Addr(const char* ip, uint16_t port) {
    bzero(&addr, len);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);
}

Socket::Socket() 
    : m_fd(-1)
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    // errif(m_fd == -1, "socket create error");
}

Socket::Socket(int fd) 
    : m_fd(fd) 
{
    // errif(fd < 0, "fd is illegal");
}

Socket::~Socket() {
    if (m_fd >= 0) {
        ::close(m_fd);
    }
}

void Socket::bind(Addr& addr) {
    ::bind(m_fd, (struct sockaddr*)&addr, addr.len);
    // errif(::bind(m_fd, (struct sockaddr*)&addr, addr.len) < 0, "bind error");
}

void Socket::listen() {
    ::listen(m_fd, 1024);
    // errif(::listen(m_fd, 1024) < 0, "listen error");
}

void Socket::setNonblock() {
    int flag = fcntl(m_fd, F_GETFL);
    int new_flag = flag | O_NONBLOCK;
    fcntl(m_fd, F_SETFL, new_flag);
}

int Socket::accept(Addr& addr) {
    int fd = ::accept(m_fd, (struct sockaddr*)&addr, &addr.len);
    // errif(fd < 0, "accept error");
    return fd;
}