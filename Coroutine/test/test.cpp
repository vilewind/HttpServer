#include "coroutine.h"
#include <iostream>
#include <thread>

void foo1(int a) {
    for (int i = a; i < 5 + a; ++i) {
        std::cout << __func__ << " int thread " << std::this_thread::get_id() << " value is " << i << std::endl;
        Coroutine::Yield();
    }
}

void foo2() {
    for (int i = 0; i < 5; ++i) {
        std::cout << __func__ << " in thread " << std::this_thread::get_id() << "value is " << i << std::endl;
        Coroutine::Yield();
    }
}

void foo() {
     Coroutine::CoroutineSptr co1 = Coroutine::getInstanceCoroutine();
     Coroutine::CoroutineSptr co2 = Coroutine::getInstanceCoroutine();
     std::cout << std::this_thread::get_id() << " thread start" << std::endl;

    co1->setCallback(std::bind(foo1, 5));
    co2->setCallback(foo2);
    
    for (int i = 0; i < 5; ++i) {
        Coroutine::Resume(co1);
        Coroutine::Resume(co2);
    }

}

/** @TODO 输出参数并未出现变化*/
int main()
{
    std::thread t1(foo);
    std::thread t2(foo);

    t1.join();
    t2.join();

    return 0;
}

