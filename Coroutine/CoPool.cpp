#include "CoPool.h"
#include "Coroutine.h"
#include <thread>
#include <vector>
#include <iostream>
#include <cstring>

static thread_local Coroutine* t_mainCo = nullptr;
static thread_local Coroutine* t_curCo = nullptr;
static thread_local char* t_stack;
static thread_local CoPool* t_coPool = nullptr;
static thread_local std::vector<Coroutine*> t_coS;

CoPool::CoPool(){
    if (t_coPool != nullptr) {
        std::cerr << "there keeps a coroutine pool";
        exit(EXIT_FAILURE);
    }
    t_coPool = this;
    if (t_mainCo == nullptr) {
        t_mainCo = new Coroutine(this);
        t_curCo = t_mainCo;
    }
    if (t_stack == nullptr) {
        t_stack = new char[STACK_SIZE]();
    }

    for (int i = 0; i < 18; ++i) {
        t_coS.emplace_back(new Coroutine(this));
    }
}

CoPool::~CoPool() {
    if (t_mainCo)
        delete t_mainCo;
    for(int i = 0; i < t_coS.size(); ++i) {
        if (t_coS[i])
            delete t_coS[i];
    }
    t_coS.clear();
    t_coS.shrink_to_fit();
}

// CoPool* CoPool::getInstanceCoPool() {
//     // static thread_local CoPool cp;
//     if (t_coPool == nullptr)
//         CoPool();
//     return t_coPool;
// }

bool CoPool::isMainCo() {
    return t_mainCo == nullptr || t_mainCo == t_curCo;
}

Coroutine* CoPool::getMainCo() {
    // assert(t_mainCo != nullptr);
    return t_mainCo;
}

Coroutine* CoPool::getCurCo() {
    // assert(t_curCo !+ nullptr);
    return t_curCo;
}

Coroutine* CoPool::getInstanceCo() {
    int cur = 0;
    for (; cur < t_coS.size(); ++cur) {
        if (t_coS[cur] != nullptr || !t_coS[cur]->m_isUsed || t_coS[cur]->m_status == Coroutine::CO_READY)
            break;
    }

    if (cur == t_coS.size()) {
        for (int i = cur; i < cur + 18; ++i) {
            t_coS.emplace_back(new Coroutine(t_coPool));
        }
    }
    t_coS[cur]->m_isUsed = true;
    return t_coS[cur];
}

void CoPool::Resume(Coroutine* co) {
    /* m_isUsed=false == m_status=CO_DEAD*/
    if (co == nullptr || !co->m_isUsed)
        return;
    /* only main coroutine is able to resume other following coroutines*/
    if (!isMainCo()) {
        return;
    }

    if (co->m_status == Coroutine::CO_READY) {
        co->coMake();
    } else if (co->m_status == Coroutine::CO_SUSPEND){
        ::memmove(t_stack + CoPool::STACK_SIZE - co->m_size, co->m_stack, co->m_size);
    }
    t_curCo = co;
    if (t_mainCo == nullptr) {
        std::cout << __func__ << " t_mainCo is nullptr" << std::endl;
    }
    coctx_swap(&t_mainCo->m_ctx, &co->m_ctx);
}

void CoPool::Yield() {
    /* main coroutine cannot be yield*/
    if (isMainCo()) {
        return;
    }

    Coroutine* co = t_curCo;
    t_curCo = t_mainCo;
    co->m_status = Coroutine::CO_SUSPEND;
    co->stackCopy(t_stack + CoPool::STACK_SIZE);
    coctx_swap(&co->m_ctx, & t_mainCo->m_ctx);
}

char* CoPool::getSharedStack() {
    return t_stack;
}