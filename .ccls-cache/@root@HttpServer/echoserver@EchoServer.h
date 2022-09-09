/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 05:18
#
# Filename: EchoServer.h
#
# Description: 
#
=============================================================================*/
#include "Util.h"
#include <vector>

using namespace Util;
using namespace Socket;
using namespace Socket::SocketUtil;

struct epoll_event;

class Server
{
public:
    Server();
    ~Server();

    void loop();

    void epollAdd(int fd, epoll_event & ee);

    void accept();

    static void echo(int fd);
private:
    Socket::Socket m_soc;
    Addr m_addr;
    int m_epfd;
    std::vector<epoll_event> m_eventLs;
};
