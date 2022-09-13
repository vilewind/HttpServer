/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 05:53
#
# Filename: Util.cpp
#
# Description: 
#
=============================================================================*/

#include "Util.h"
#include <fcntl.h>

int Socket::SocketUtil::createNoblockSocket() {
    return Util::ERRIF(__func__, 0, ::socket, AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
}

void Socket::SocketUtil::halfClose(int fd, int op) {
    const char* err = nullptr;
    if (op == SHUT_RD) {
        err = "shutdown read";
    } else if (op == SHUT_WR) {
        err = "shutdown write";
    } else {
        err = "shutdown read and write";
    }
    Util::ERRIF(err, 0, ::shutdown, fd, op);
}

void Socket::SocketUtil::setNonblock(int fd) {
	int flag = fcntl(fd, F_GETFL);
	int new_flag = flag | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_flag);
}
