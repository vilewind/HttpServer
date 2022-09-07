/*=============================================================================
#
# Author: vilewind - luochengx2018@163.com
#
# Last modified: 2022-09-06 08:09
#
# Filename: TimerWheel.cpp
#
# Description: 
#
=============================================================================*/
#include "TimerWheel.h"

void TimerWheel::caculateTimer(TimerSp timer) {
	if (timer == nullptr)
		return;

	int ticks = 0;
	int timeout = timer->timeout;

	if (timeout < SI) {
		ticks = 1;
	} else {
		ticks = timeout / SI;
	}
/* 计算待触发的事件在时间轮转动多少圈后被触发*/
	timer->rotation = ticks / WHEEL_SIZE;
/* 计算对应槽位*/
	timer->slot = (m_slot + (ticks%WHEEL_SIZE)) % WHEEL_SIZE;
}

void TimerWheel::addTimer(TimerSp timer) {
	if (timer == nullptr)
	{
		std::cout << "timer is nullptr" << std::endl;
		return;
	}
	caculateTimer(timer);
	if (!m_timers[timer->slot]) {
		m_timers[timer->slot] = timer;
	} else {
		timer->next = m_timers[timer->slot];     
		m_timers[timer->slot]->prev = timer;
		m_timers[timer->slot] = timer;
	}
}

void TimerWheel::removeTimer(TimerSp timer) {
	if (timer == nullptr) {
		std::cout << "timer is nullptr" << std::endl;
	}

	int slot = timer->slot;
	if (slot < 0) {
		std::cout << "timer is not in timer wheel" << std::endl;
		return;
	}

	if (timer == m_timers[slot]) {
		m_timers[slot] = timer->next;
		if (timer->next != nullptr) {
			timer->next->prev = nullptr;
		}
		timer->next = nullptr;
	} else {
		if(timer->prev == nullptr) return;
		timer->prev->next = timer->next;
		if (timer->next != nullptr) {
			timer->next->prev = timer->prev;
		}
		timer->next = timer->prev = nullptr;
	}
	
}

void TimerWheel::adjustTimer(TimerSp timer) {
	if (timer == nullptr) {
		std::cout << "timer is nullptr" << std::endl;
		return;
	}
	removeTimer(timer);
	caculateTimer(timer);
	addTimer(timer);
}




































