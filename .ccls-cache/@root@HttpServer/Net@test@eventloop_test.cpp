/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-05 09:01
#
# Filename: eventloop_test.cpp
#
# Description: 
#
=============================================================================*/
#include "../EventLoop.h"
#include <iostream>

EventLoop* g_loop;

void threadFunc2() {
	g_loop->loop();
}

void threadFunc() {
	std::cout << "threafunc(): " << std::this_thread::get_id() << std::endl;	
	EventLoop loop;
	loop.loop();
}

int main() {
	std::cout << "main(): " << std::this_thread::get_id() << std::endl;

	EventLoop loop;
	g_loop = &loop;
	std::thread t(threadFunc);
	std::thread t1(threadFunc2);
	loop.loop();
	t.join();
	t1.join();

	return 0;
	
}
