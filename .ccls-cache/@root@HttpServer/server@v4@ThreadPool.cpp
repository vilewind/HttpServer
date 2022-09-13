/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-13 12:20
#
# Filename: ThreadPool.cpp
#
# Description: 
#
=============================================================================*/
#include "ThreadPool.h"
#include <cstdio>

template<typename T>
void ThreadSafeQueue<T>::push(T value) {
	std::lock_guard<std::mutex> locker(m_mu);
	m_que.push(value);
	m_cv.notify_one();
}

template<typename T>
void ThreadSafeQueue<T>::wait_and_pop(T& value) {
	std::unique_lock<std::mutex> locker(m_mu);
	m_cv.wait(locker, [this]
			 { return !m_que.empty(); });
	value = std::move(m_que.front());
	m_que.pop();
}

template<typename T>
bool ThreadSafeQueue<T>::try_pop(T& value) {
	std::lock_guard<std::mutex> locker(m_mu);
	if (m_que.empty()) {
		return false;
	} else {
		value = std::move(m_que.front());
		m_que.pop();
		return true;
	}
}

template<typename T>
bool ThreadSafeQueue<T>::empty() {
/** @bug const 与 m_mu冲突*/
//bool ThreadSafeQueue<T>::empty() const{
	std::lock_guard<std::mutex> locker(m_mu);
	return m_que.empty();
}

template<class Object>
ThreadPool<Object>::ThreadPool(int thread_num)
	: m_stop(false)
 {
	 if (thread_num >= std::thread::hardware_concurrency())
		 thread_num = std::thread::hardware_concurrency();
	 try{
		for (int i = 0; i < thread_num; ++i) {
			m_threads.push_back(std::thread{&ThreadPool::threadFunc, this});
		}

		printf("threads size %lu\n", m_threads.size());
	 } catch(...) {
		 m_stop = true;
		 throw;
	 }
 }

template<class Object>
ThreadPool<Object>::~ThreadPool() {
/* 等待任务完成*/
	m_stop = true;
	for (int i = 0; i < m_threads.size(); ++i)
	{
		if (m_threads[i].joinable())
		{
			m_threads[i].join();
		}
	}
} 

template<class Object>
void ThreadPool<Object>::threadFunc() {
	/* static thread_local 初始化一次*/
	static thread_local Object ob;
	m_objects.push_back(ob);
	if (m_stop)
		printf("e\n");
	while(!m_stop) {
		Task task;
		if (m_tasks.try_pop(task)) {
			printf("execute task\n");
			task();
		}
		else {
			//printf("no task\n");
			std::this_thread::yield();
		}
	}
}

template<class Object>
std::vector<Object>* ThreadPool<Object>::getObjects() {
	return &m_objects;
}

