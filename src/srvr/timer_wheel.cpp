
#include <unordered_map>
#include <mutex>
#include <thread>

#include "timer_wheel.h"
#include "memdb.h"
#include "logger.h"

TmrWheel::TmrWheel(MemDB* pmemdb) : 	m_mtx{}
						,m_tmr_wheel{}
						,m_tmr_thr{new std::jthread{&TmrWheel::manage_timers,this}}
						,m_memDB(pmemdb)
{
}
bool TmrWheel::manage_timers()
{
	std::chrono::milliseconds currsecs{};

	for ( ; ; ) {	

		std::unique_lock<std::mutex> lck(m_mtx);
		while (m_tmr_wheel.empty()) {
			m_cv.wait(lck,[this]{return !m_tmr_wheel.empty();});
		}

		currsecs=m_tmr_wheel.front().first;	
		auto now=std::chrono::system_clock::now().time_since_epoch();
		auto expire=now + currsecs;
		while ((m_tmr_wheel.front().first == currsecs) 
				&& (expire > now)) {
			m_cv.wait_for(lck
					,currsecs
					,[=,this]{return (m_tmr_wheel.front().first!=currsecs);});
			now=std::chrono::system_clock::now().time_since_epoch();
			Logger::instance().log_info(" after wait_for. m_tmr_wheel.size:" ,std::to_string(m_tmr_wheel.size())," now is:",std::to_string(now.count()));
			print_tmr_wheel();
			if (m_tmr_wheel.front().first != currsecs) {
				Logger::instance().log_info(" updating tmr_wheel front. front:" 
							,std::to_string(m_tmr_wheel.front().first.count())
							," update from currsecs: " 
							,std::to_string(currsecs.count()));
				currsecs=m_tmr_wheel.front().first;
				now=std::chrono::system_clock::now().time_since_epoch();
				expire=now + currsecs;
				continue;
			}
			else {
			//	break;
			}
		}
	
		auto elem2rmv = m_tmr_wheel.front();
		m_tmr_wheel.pop_front();
		Logger::instance().log_info(" removed element with expired time:" 
					, std::to_string(elem2rmv.first.count())
					," with key:",elem2rmv.second->first);
		m_memDB->remove(elem2rmv.second->first);
	}
}
void TmrWheel::print_tmr_wheel()
{
	for (auto item:m_tmr_wheel) {
		Logger::instance().log_info(" expire:",std::to_string(item.first.count())," key:",item.second->first);
	}
}
bool TmrWheel::insert(int expiresec,MemDB::Hash_t::iterator expireobj)
{
	std::lock_guard<std::mutex>	wrtrLck(m_mtx);
	std::pair<std::chrono::milliseconds,MemDB::Hash_t::iterator> newTimer{expiresec*1000,expireobj};
	Logger::instance().log_info("TmrWheel::insert RCV newTimer expire time is:",std::to_string(newTimer.first.count()));
	if (newTimer.first.count() < 0) {
		newTimer.first = std::chrono::milliseconds(INT_MAX);
	}
	if (m_tmr_wheel.empty()) {
		m_tmr_wheel.push_front(newTimer);
		Logger::instance().log_info(" insert to empty wheel newTimer expire time is:", std::to_string(newTimer.first.count()));
		m_cv.notify_all();
		return true;
	}
	if (m_tmr_wheel.front().first > newTimer.first) {
		m_tmr_wheel.front().first -= newTimer.first;
		m_tmr_wheel.push_front(newTimer);
		Logger::instance().log_info(" insert in front of wheel newTimer expire time is:", std::to_string(newTimer.first.count()));
		m_cv.notify_all();
		return true;
	}
	auto itrcurr = m_tmr_wheel.begin();
	auto itrprev = m_tmr_wheel.begin();
	auto itrend = m_tmr_wheel.end();
	for ( ; itrcurr != itrend; ++itrcurr) {
		if ((*itrcurr).first < newTimer.first) {
			newTimer.first -= (*itrcurr).first;
			itrprev=itrcurr;
			continue;
		}
		Logger::instance().log_info(" insert in middle of wheel newTimer expire time is:", std::to_string(newTimer.first.count())," after timer:", std::to_string(itrprev->first.count()));
		(*itrcurr).first -= newTimer.first;
		m_tmr_wheel.insert(itrprev,newTimer);
		return true;
	}

	return false;
}
