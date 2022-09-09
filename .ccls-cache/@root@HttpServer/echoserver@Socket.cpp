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
#include "../include/Util.h"
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
    m_fd = ERRIF("socket", 0, socket, AF_INET, SOCK_STREAM, 0);
}

Socket::Socket(int fd) 
    : m_fd(fd) 
{
	if (m_fd < 0) {
		std::cerr << "fd is illegal";
		exit(EXIT_FAILURE);
	}
}

Socket::~Socket() {
    if (m_fd >= 0) {
        ::close(m_fd);
    }
}

void Socket::bind(Addr& addr) {
    ERRIF("bind", 0, ::bind, m_fd, (struct sockaddr*)&addr, addr.len);
}

void Socket::listen() {
    ERRIF("listen", 0, ::listen, m_fd, 1024);
}

void Socket::setNonblock() {
    int flag = fcntl(m_fd, F_GETFL);
    int new_flag = flag | O_NONBLOCK;
    fcntl(m_fd, F_SETFL, new_flag);
}

int Socket::accept(Addr& addr) {
    int fd = ERRIF("accept", 0, ::accept, m_fd, (struct sockaddr*)&addr, &addr.len);
    return fd;
}
