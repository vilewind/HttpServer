/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 05:41
#
# Filename: main.cpp
#
# Description: 
#
=============================================================================*/

#include "Util.h"
#include <sys/epoll.h>
#include <vector>
#include <strings.h>
#include <cerrno>

using namespace std;
using namespace Socket;
using namespace Socket::SocketUtil;

int main() {
    Addr addr("127.0.0.1", 8888);
    Socket::Socket soc(createNoblockSocket());
    soc.bind(addr);
    soc.listen();

    vector<epoll_event> events;
    events.reserve(1024);

    int epfd = ::epoll_create1(0);
    if (epfd < 0) {
        cerr << "epoll create";
        exit(EXIT_FAILURE);
    }

    epoll_event ev;
    bzero(&ev, sizeof ev);
    ev.data.fd = soc.getFd();
    ev.events = EPOLLIN | EPOLLET;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, soc.getFd(), &ev);
    if (ret < 0) {
        cerr << "epoll add";
        exit(EXIT_FAILURE);
    }

    while(true) {
        int nums = epoll_wait(epfd, &*events.begin(), events.capacity(), -1);
        if (nums < 0) {
            cerr << "epoll wait";
            exit(EXIT_FAILURE);
        } else if (nums == 0) {
            cout << "nothing happened" << endl;
        } else {
            for (int i = 0; i < nums; ++i) {
                epoll_event ee = events[i];
                if (ee.data.fd == soc.getFd()) {
                    epoll_event ce;
                    Addr ca;
                    int fd = soc.accept(&ca);
                    ce.data.fd = fd;
                    ce.events = EPOLLIN | EPOLLET;
                    setNonblock(fd);
                    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ce);
                } else {
                    if (ee.events & EPOLLIN) {
                        char buf[1024];
                        while(1) {
                            bzero(buf, 1024);
                            ssize_t n = ::read(ee.data.fd, buf, sizeof buf);
                            if (n == -1) {
                                if (errno == EINTR) {
                                    continue;
                                } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                    break;
                                }
                            } else if (n == 0) {
                                ::close(ee.data.fd);
                            } else {
                                ::write(ee.data.fd, buf, sizeof buf);
                            }
                        }
                    }
                }
            }
        }
    }


}