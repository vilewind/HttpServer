/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-13 12:17
#
# Filename: ThreadPool.h
#
# Description: 
#
=============================================================================*/
#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <functional>
#include <vector>
#include <thread>
#include <atomic>
#include <future>
#include <queue>
#include <mutex>
#include <condition_variable>

template<class T>
class ThreadSafeQueue
{
public:
	ThreadSafeQueue()=default;
	~ThreadSafeQueue()=default;

	void push(T);
	void wait_and_pop(T &);
	bool try_pop(T&);
	//bool empty() const;
	bool empty();
private:
	std::mutex m_mu{};
	std::condition_variable m_cv{};
	std::queue<T> m_que{};
};

template<class Object>
class ThreadPool
{
public:
	using Task = std::function<void()>;
	ThreadPool(int thread_num = 10);
	//WorkerThreadPool(const WorkerThreadPool &) = delete;
	//WorkerThreadPool& operator=(const WorkerThreadPool &) = delete;

	~ThreadPool();
	void threadFunc();
	
	std::vector<Object>* getObjects();


	template <typename Func, typename... Args>
	auto submit(Func &&f, Args &&...args) -> std::future<decltype(f(args...))>;
private:
	std::vector<std::thread> m_threads;
	ThreadSafeQueue<Task> m_tasks;
	std::atomic<bool> m_stop;
	std::vector<Object> m_objects;
	
};

/**
 * @brief 可异步获取结果的提交函数。由于是模版编程，因此在.h文件中声明并定义
 * 
 * @tparam Func 
 * @tparam Args 
 * @param f 
 * @param args 
 * @return std::future<decltype(f(args...))> 
 */
template<class Object>
template <typename Func, typename... Args>
auto ThreadPool<Object>::submit(Func&& f, Args &&... args)->std::future<decltype(f(args...))> {
	using FUNCTYPE = decltype(f(args...));
	std::function<FUNCTYPE()> func = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
/** @attention c++11异步编程，packaged_task与future结合*/	
	auto task_ptr = std::make_shared<std::packaged_task<FUNCTYPE()>>(func);
/** @attention 包装函数，可以统一任务列表；同时对于一些传统的linux接口，可以使用这种方法结合function*/
	Task task = [task_ptr]()
	{
		(*task_ptr)();
	};

	m_tasks.push(task);

	return task_ptr->get_future();
}



#endif

