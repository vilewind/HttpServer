#include "coroutine.h"
#include "coctx.h"
#include <cstring>
#include <memory>
#include <cassert>
#include <cstdlib>
#include <thread>
#include <vector>
#include <iostream>

static thread_local Coroutine::CoManager* t_coMananger = nullptr;
static thread_local Coroutine* t_mainCoroutine = nullptr;
static thread_local Coroutine* t_curCoroutine = nullptr;
static thread_local char* t_stack;
static thread_local std::vector<Coroutine*> t_coroutinePool;
const static int STACK_SIZE = 1024 * 512;

Coroutine::CoManager::CoManager() {
    /* 强制一个线程中只存在一个协程池，并且t_mainCoroutine只在协程池建立时初始化*/
    if (t_coMananger != nullptr || t_mainCoroutine != nullptr || t_curCoroutine != nullptr) {
        std::cerr << "there keeps another coroutine manager";
        exit(EXIT_FAILURE);
    }
    t_coMananger = this;
    t_mainCoroutine = new Coroutine();
    t_curCoroutine = t_mainCoroutine;
    // t_stack = new char[STACK_SIZE]();
    t_stack = (char*)malloc(STACK_SIZE);
}

Coroutine::CoManager::~CoManager() {
    if (t_mainCoroutine)
        delete t_mainCoroutine;
    if (t_stack)
        free(t_stack);
        // delete[] t_stack;
    for (int i = 0; i < t_coroutinePool.size(); ++i) {
        if (t_coroutinePool[i])
            delete t_coroutinePool[i];
    }
    t_coroutinePool.clear();
    t_coroutinePool.shrink_to_fit();
}

void Coroutine::CoFunc(Coroutine *co) {
    Coroutine::Callback cb = co->getCallback();
    if (cb) 
        cb();
    Coroutine::Yield();
}

Coroutine::Coroutine()
{
    memset(&ctx_, 0, sizeof ctx_);
    /* if (t_mainCoroutine == nullptr) { */
    /*     t_mainCoroutine = this; */
    /*     t_curCoroutine = t_mainCoroutine; */
    /* } */
}

Coroutine::~Coroutine() {
    if (stack_sp_) 
        free(stack_sp_);
        // delete[] stack_sp_;
    is_used_ = false;
    status_ = CO_READY;
}

void Coroutine::stackCopy(char* top) {
    char dummy = 0;
    assert(top - &dummy <= STACK_SIZE);
    if (cap_ < top - &dummy) {
        if (stack_sp_)
            free(stack_sp_);
        cap_ = top - &dummy;
        // stack_sp_ = new char[cap_]();
        stack_sp_ = nullptr;
        stack_sp_ = (char*)malloc(cap_);
    }

    size_ = top - &dummy;
    memcpy(stack_sp_, &dummy, size_);
}

void Coroutine::coroutineMake() {
    stack_sp_ = t_stack;
    size_ = STACK_SIZE;
    char* top = stack_sp_ + size_;
    top = reinterpret_cast<char *>((reinterpret_cast<unsigned long>(top)) & -16LL);

    memset(&ctx_, 0, sizeof ctx_);
    ctx_.regs[kRSP] = top;
    ctx_.regs[kRBP] = top;
    ctx_.regs[kRETAddr] = reinterpret_cast<char*>(CoFunc);
    ctx_.regs[kRDI] = reinterpret_cast<char*>(this);
}

bool Coroutine::isMainCoroutine() {
    if (t_mainCoroutine == nullptr || t_mainCoroutine == t_curCoroutine)
        return true;
    return false;
}

void Coroutine::setCallback(Callback cb) {
    cb_ = cb;
    getMainCoroutine();
}

Coroutine* Coroutine::getMainCoroutine() {
    /* CoMananger建立时，对应的t_mainCoroutine已经生成*/
    // if (t_mainCoroutine == nullptr) {
    //     t_mainCoroutine = std::make_shared<Coroutine>();
    //     t_curCoroutine = t_mainCoroutine;
    // }
    assert(t_mainCoroutine != nullptr);
    return t_mainCoroutine;
}

Coroutine* Coroutine::getCurCoroutine() {
    // if (t_curCoroutine == nullptr)
    //     getMainCoroutine();
    assert(t_curCoroutine != nullptr);
    return t_curCoroutine;
}

Coroutine* Coroutine::getInstanceCoroutine() {
    int cur = 0;
    for (; cur < t_coroutinePool.size() && !t_coroutinePool.empty(); ++cur) {
        if (t_coroutinePool[cur] != nullptr && t_coroutinePool[cur]->is_used_ == false && t_coroutinePool[cur]->status_ == CO_READY) 
            break;
    }
    
    if (cur >= t_coroutinePool.size()) {
        for (int i = cur; i < cur + 16; ++i) {
            Coroutine* co = new Coroutine();
            t_coroutinePool.push_back(co);
        }
    }
    std::cout << "get instance coroutine" << std::endl;
    t_coroutinePool[cur]->is_used_ = true;
    return t_coroutinePool[cur];
}

void Coroutine::Resume(Coroutine* co) {
    if (co == nullptr || co->status_ == CO_DEAD) {
        /* target coroutine is nullptr*/
        return;
    }

    if (!isMainCoroutine()) {
        /* current coroutine is not main coroutine without the ability to resume sub-couroutine*/
        return;
    }

    if (co->status_ == CO_READY)
        co->coroutineMake();
    else if (co->status_ == CO_SUSPEND) {
        memcpy(t_stack + STACK_SIZE - co->size_, co->stack_sp_, co->size_);
    }
    t_curCoroutine = co;
    coctx_swap(&t_mainCoroutine->ctx_, &co->ctx_);
}

void Coroutine::Yield() {
    if (isMainCoroutine()) {
        /* main coroutine cannot Yield*/
        return;
    }

    Coroutine* co = t_curCoroutine;
    t_curCoroutine = t_mainCoroutine;
    co->status_ = CO_SUSPEND;
    co->stackCopy(t_stack + STACK_SIZE);
    coctx_swap(&co->ctx_, &t_mainCoroutine->ctx_);
}


/*=================================================test=================================================*/

void foo1(int a) {
    for (int i = a; i < 1 + a; ++i) {
        std::cout << __func__ << " int thread " << std::this_thread::get_id() << " value is " << i << std::endl;
        Coroutine::Yield();
    }
}

void foo2() {
    for (int i = 0; i < 1; ++i) {
        std::cout << __func__ << " in thread " << std::this_thread::get_id() << "value is " << i << std::endl;
        Coroutine::Yield();
    }
}

void foo() {
    Coroutine::CoManager cm; 
     Coroutine* co1 = Coroutine::getInstanceCoroutine();
     Coroutine* co2 = Coroutine::getInstanceCoroutine();
     std::cout << std::this_thread::get_id() << " thread start" << std::endl;

    co1->setCallback(std::bind(foo1, 1));
    co2->setCallback(foo2);
    
    for (int i = 0; i < 2; ++i) {
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

