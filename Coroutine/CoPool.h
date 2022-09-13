#ifndef __COPOOL_H__
#define __COPOOL_H__

class Coroutine;

class CoPool
{
public:
    CoPool();
    ~CoPool();

    static CoPool* getInstanceCoPool();

    static bool isMainCo();

    static Coroutine* getMainCo();

    static Coroutine* getCurCo();

    static Coroutine* getInstanceCo();

    static void Resume(Coroutine*);

    static void Yield();

    static char* getSharedStack();

    const static int STACK_SIZE = 1024 * 512;
};

#endif
