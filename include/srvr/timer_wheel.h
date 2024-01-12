
#ifndef __TIMER_WHEEL_H
#define __TIMER_WHEEL_H

#include <chrono>
#include <list>
#include <unordered_map>
#include <utility>
#include <iostream>
//#include <shared_mutex>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "memdb.h"


class TmrWheel
{
public:

	using MemDBHashIter_t = MemDB::Hash_t::iterator;

	explicit TmrWheel(MemDB* memdb);
	~TmrWheel() noexcept { while (!m_tmr_wheel.empty()) m_tmr_wheel.pop_front();}

	bool manage_timers();
	bool insert(int,MemDBHashIter_t);
	void print_tmr_wheel();

protected:


	std::mutex	m_mtx;
	std::list<std::pair<std::chrono::milliseconds,MemDBHashIter_t>> m_tmr_wheel;
	std::unique_ptr<std::jthread>	m_tmr_thr;
	std::condition_variable	m_cv;
	MemDB*	m_memDB;

private:
};

#endif
