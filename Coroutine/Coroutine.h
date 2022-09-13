#ifndef __COROUTINE_H__
#define __COROUTINE_H__

#include "coctx.h"
#include <functional>
#include <cstddef>

class CoPool;

class Coroutine
{
public:
    using CoTask = std::function<void()>;
    enum Status
    {
        CO_READY = 0,
        CO_SUSPEND,
        CO_RUNNING,
        CO_DEAD
    };

    Coroutine(CoPool*);
    ~Coroutine();

    void setTask(CoTask ct) { m_ct = ct; }
    CoTask getTask() const { return m_ct; }

    void coMake();

    void stackCopy(char* top);

    static void execTask(Coroutine*);

public:
    CoPool* m_pool;
    char* m_stack{nullptr};
    ptrdiff_t m_capacity{0};
    ptrdiff_t m_size{0};
    bool m_isUsed{false};
    Status m_status{CO_READY};
    coctx_t m_ctx;
private:
    CoTask m_ct;
};

#endif