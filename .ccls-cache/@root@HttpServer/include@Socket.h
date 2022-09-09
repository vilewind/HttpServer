/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-07 11:33
#
# Filename: Socket.h
#
# Description: 简单封装sockeaddr和socket类，提供bind、accept等基础接口
#
=============================================================================*/

#ifndef __SOCEKT_H__
#define __SOCEKT_H__
#include <arpa/inet.h>
#include <string>

struct Addr
{
    struct sockaddr_in addr;
    socklen_t len{sizeof(addr)};

    Addr() = default;
    Addr(const char* ip, uint16_t port);
    ~Addr() = default;
};

class Socket
{
public:
    Socket();
    Socket(int);
    ~Socket();

    void bind(Addr&);
    
    void listen();

    void setNonblock();

    int accept(Addr&);

    int getFd() const { return m_fd; }
private:
    int m_fd{-1};
};

#endif
