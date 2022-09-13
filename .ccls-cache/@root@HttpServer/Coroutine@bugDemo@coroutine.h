#ifndef __COROUTINE_H__
#define __COROUTINE_H__
#include "coctx.h"
#include <cstddef>
#include <functional>

class Coroutine;

class Coroutine
{
public:
    using Callback = std::function<void()>;
    
    enum Status
    {
        CO_READY = 0,
        CO_SUSPEND,
        CO_RUNNING,
        CO_DEAD
    };

    struct CoManager
    {
        CoManager();
        ~CoManager();
    };
public:
	Coroutine();
	~Coroutine();
    
    void setCallback(Callback);
    Callback getCallback() const { return cb_; }
    static void Resume(Coroutine*);
    static void Yield();

    static bool isMainCoroutine();

    static Coroutine* getMainCoroutine();
    static Coroutine* getCurCoroutine();
    static Coroutine* getInstanceCoroutine();
    static void CoFunc(Coroutine *);
private:
    void coroutineMake();
    void stackCopy(char* top);
public:
    coctx_t ctx_;
    char* stack_sp_{nullptr};
    ptrdiff_t cap_{0};
    ptrdiff_t size_{0};
	bool is_used_{false};
    Status status_{CO_READY};
private:    
    Callback cb_;
};

#endif

