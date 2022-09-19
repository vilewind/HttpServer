/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-19 06:47
#
# Filename: main.cpp
#
# Description: 
#
=============================================================================*/

#define ECHOSERVER

#ifdef ECHOSERVER
#include "EchoServer.h"
#endif
#include "net/EventLoop.h"
#include <signal.h>

int main()
{  
/* 忽略sigpipe信号*/     
    ::signal( SIGPIPE, SIG_IGN );

    EventLoop loop;
#ifdef ECHOSERVER
    EchoServer es( &loop);
    es.start();
#endif
    loop.loop();

    return 0;
}
