/*=============================================================================
#
# Author: vilewind - luochengx2018@163.com
#
# Last modified: 2022-09-05 11:51
#
# Filename: Util.h
#
# Description:基础工具类
#
=============================================================================*/
#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <unistd.h>

/**
 * @brief 实现不可复制类，但可移动
*/
class noncopyable
{
public:
	noncopyable() = default;
	~noncopyable() = default;
	
	noncopyable(const noncopyable&) = delete;
	noncopyable& operator=(const noncopyable) = delete;
};

/**
 * @brief 错误提示
 * @param err 错误信息
 * @param flag 判断是否错误的标志
 * @param func 执行的函数，如socket等
 * @param args 执行函数的必要参数
 * 
 * @returns 执行函数的返回值
*/
template<typename Func, typename ...Args>
auto ERRIF(const char* err, int flag, Func&& func, Args&& ...args)->decltype(func(args...)) {
	auto res = func(args...);
	if (res < flag) {
		std::cerr << err;
		exit(EXIT_FAILURE);
	} 
	return res;
}

#endif

