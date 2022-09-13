#include <iostream>
#include <thread>
#include "CoPool.h"
#include "Coroutine.h"

void foo1(int a) {
    for (int i = a; i < 1 + a; ++i) {
        std::cout << __func__ << " int thread " << std::this_thread::get_id() << " value is " << i << std::endl;
        CoPool::Yield();
    }
}

void foo2() {
    for (int i = 0; i < 1; ++i) {
        std::cout << __func__ << " in thread " << std::this_thread::get_id() << " value is " << i << std::endl;
        CoPool::Yield();
    }
}

void foo() {
    CoPool cp;
     Coroutine* co1 = CoPool::getInstanceCo();
     Coroutine* co2 = CoPool::getInstanceCo();
     std::cout << std::this_thread::get_id() << " thread start" << std::endl;

    co1->setTask(std::bind(foo1, 1));
    co2->setTask(foo2);
    
    for (int i = 0; i < 2; ++i) {
        CoPool::Resume(co1);
        CoPool::Resume(co2);
    }

}

/** @TODO 输出参数并未出现变化*/
int main()
{
    // CoPool *cp = new CoPool();
    std::thread t1(foo);
    // std::thread t2(foo);

    t1.join();
    // t2.join();

    // delete cp;
    return 0;
}

