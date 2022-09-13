/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 09:49
#
# Filename: main.cpp
#
# Description: 
#
=============================================================================*/

#include "EventLoop.h"
#include "Server.h"

int main() {
    EventLoop loop;
    Server server(&loop);

    loop.loop();

    return 0;
}