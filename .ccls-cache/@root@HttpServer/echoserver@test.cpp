/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-08 11:26
#
# Filename: test.cpp
#
# Description: 
#
=============================================================================*/
#include "EventLoop.h"

void threadFunc() {
	std::cout << __func__ << " " << std::this_thread::get_id() << std::endl;
	EventLoop loop;
	loop.loop();
}

int main() {
	std::cout << __func__ << " " << std::this_thread::get_id() << std::endl;
	EventLoop loop;
	loop.loop();
	threadFunc();

	exit(EXIT_SUCCESS);
}



