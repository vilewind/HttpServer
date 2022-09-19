#include "Coroutine.h"
#include "CoPool.h"
#include <cstring>
#include <cassert>

Coroutine::Coroutine(CoPool* cp)
    : m_pool(cp)
{
    ::memset(&m_ctx, 0, sizeof m_ctx);
}

Coroutine::~Coroutine() {
    if (m_stack)
        delete[] m_stack;
    m_status = CO_READY;
    m_isUsed = false;
}

void Coroutine::execTask(Coroutine* co) {
    Coroutine::CoTask ct = co->getTask();
    if (ct)
        ct();
    CoPool::Yield();
}

void Coroutine::coMake() {
    m_stack = CoPool::getSharedStack();
    m_size = CoPool::STACK_SIZE;

    /* 堆上栈的栈底*/
    char* top = reinterpret_cast<char*>((reinterpret_cast<unsigned long>(m_stack + m_size)) & -16LL);

    ::memset(&m_ctx, 0, sizeof m_ctx);
    m_ctx.regs[kRSP] = top;
    m_ctx.regs[kRBP] = top;
    m_ctx.regs[kRETAddr] = reinterpret_cast<char*>(execTask);
    m_ctx.regs[kRDI] = reinterpret_cast<char*>(this);
}

/**
 * @brief 复制从协程的“栈”到共享栈中
 * @param top = shared_stack + shared_stack_size
*/
void Coroutine::stackCopy(char* top) {
    /* 栈顶位置，堆上栈顶低地址*/
    char dummy = 0;
    ptrdiff_t capacity = top - &dummy;
    assert(capacity <= CoPool::STACK_SIZE);

    if (m_capacity < capacity) {
        // if (m_stack)
        //     delete[] m_stack;
        m_stack = nullptr;
        m_capacity = capacity;
        m_stack = new char[capacity]();
        }

    m_size = capacity;
    ::memmove(CoPool::getSharedStack(), &dummy, m_size);
}