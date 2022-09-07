/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-07 12:31
#
# Filename: main.cpp
#
# Description: 
#
=============================================================================*/

#include "EventLoop.h"
#include "Server.h"

int main() {
    EventLoop* loop = new EventLoop();
    Server* server = new Server(loop);
    loop->loop();

    return 0;
}