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

class noncopyable
{
public:
	noncopyable() = default;
	~noncopyable() = default;
	
	noncopyable(const noncopyable&) = delete;
	noncopyable& operator=(const noncopyable) = delete;
};

#endif

