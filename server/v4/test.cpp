/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-15 03:32
#
# Filename: test.cpp
#
# Description: 
#
=============================================================================*/
#include "EchoServer.h"
#include "EventLoop.h"

int main()
{
	EventLoop loop;
	while(1) {
		static EchoServer es(&loop);
	}
	return 0;
}
