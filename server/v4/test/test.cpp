/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-13 12:49
#
# Filename: test.cpp
#
# Description: 
#
=============================================================================*/
#include <iostream>
#include <thread>
#include <unistd.h>

struct Demo;

thread_local Demo* t_demo = nullptr;

struct Demo
{
	int a = 1;
	std::thread::id tid = std::this_thread::get_id();

	Demo() { 
		if (t_demo == nullptr)
			t_demo = this;
		else 
		{
			std::cerr << "error";
			exit(EXIT_FAILURE);
		}
	}

	~Demo() {
		if (t_demo)
			t_demo = nullptr;
	}
};


void func() {
	static thread_local Demo demo;
	if (!t_demo)
		std::cout << "fail" << std::endl;
	
	for (int i = 0; i < 3; ++i) 
	{
		std::cout << demo.tid << " " << demo.a++ << std::endl;

	}
}

int main() {
	std::thread t1(func);
	std::thread t2(func);
	

	t1.join();
	t2.join();

	return 0;
}
