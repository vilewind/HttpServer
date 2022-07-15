---
基于libco中coctx_swap.S完成的协程池

---

# 协程模式

主从+共享栈



# 文件构成

- coctx_swap.S

  使用汇编语言完成主从协程的上下文切换

- coctx.h

  定义协程栈和临时存储寄存器值的结构体

- coroutine.h coroutine.cpp

  使用coctx_swap函数完成yiled和resume。使用thread_local特性，构造协程池。

# 编译构造

- 将汇编语言编译程静态库

  ```S
  g++ -c coctx_swap.S -o coctx_swap.o
  ar -crv libcoctx.a coctx_swap.o
  g++ -g *.cpp -L/path -lcoctx
  ```

  


