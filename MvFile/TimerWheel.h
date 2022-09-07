/*=============================================================================
#
# Author: vilewind - luochengx2018@163.com
#
# Last modified: 2022-09-06 05:24
#
# Filename: TimerWheel.h
#
# Description: 
#
=============================================================================*/
#ifndef __TIMERWHEEL_H__
#define __TIMERWHEEL_H__

#include <functional>
#include <vector>
#include <memory>
#include <vector>
#include "Util.h"

struct Timer 
{
	using TimerSp = std::shared_ptr<Timer>;
	using Callback = std::function<void()>;
	/* @brief 定时器类型：
	 * @mem ONCE_TIMER 触发一次后移除
	 * @mem CYCLE_TIMER 周期性触发
	 * */
	enum class TimerType
	{
		ONCE_TIMER = 0,
		CYCLE_TIMER
	};

	int timeout;

	TimerType timertype;

	Callback cb;

	int rotation{-1};

	int slot{-1};

	TimerSp prev;
	TimerSp next;

	Timer(int, TimerType, const Callback&);
	~Timer();
};

class TimerWheel : public noncopyable
{
public:
	using Callback = std::function<void()>;
	using TimerSp = std::shared_ptr<Timer>;
	using TimerContainer = std::vector<TimerSp>;

	TimerWheel() = default;
	~TimerWheel() = default;

	void addTimer(TimerSp);

	void removeTimer(TimerSp);

	void adjustTimer(TimerSp);
private:
	void caculateTimer(TimerSp);

	int m_slot{0};	//

	TimerContainer m_timers;

	static const int WHEEL_SIZE = 60;		//时间轮槽数
	static const int SI = 1;				//时间间隔
};

#endif

































