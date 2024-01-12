
#include <unordered_map>
#include <mutex>
#include <thread>

#include "timer_wheel.h"
#include "memdb.h"

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
					// && !(expire>std::chrono::system_clock::now().time_since_epoch());});
			now=std::chrono::system_clock::now().time_since_epoch();
			std::cout << "INFO TmrWheel::manage_timers after wait_for. m_tmr_wheel.size:" << m_tmr_wheel.size() << " now is:" << now.count() << std::endl;
			print_tmr_wheel();
			if (m_tmr_wheel.front().first != currsecs) {
				std::cout 	<< "INFO: updating tmr_wheel front. front:" 
							<< m_tmr_wheel.front().first.count()
							<< " update from currsecs: " 
							<< currsecs.count() << std::endl;
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
		std::cout << "INFO: removed element with expired time:" 
					<< elem2rmv.first.count() 
					<< " with key:" << elem2rmv.second->first << std::endl;
		m_memDB->remove(elem2rmv.second->first);
	}
}
void TmrWheel::print_tmr_wheel()
{
	for (auto item:m_tmr_wheel) {
		std::cout << " expire:" << item.first.count() << " key:" << item.second->first << std::endl;
	}
}
bool TmrWheel::insert(int expiresec,MemDB::Hash_t::iterator expireobj)
{
	std::lock_guard<std::mutex>	wrtrLck(m_mtx);
	std::pair<std::chrono::milliseconds,MemDB::Hash_t::iterator> newTimer{expiresec*1000,expireobj};
	std::cout << "INFO: TmrWheel::insert RCV newTimer expire time is:" << newTimer.first.count() << std::endl;
	if (newTimer.first.count() < 0) {
		newTimer.first = std::chrono::milliseconds(INT_MAX);
	}
	if (m_tmr_wheel.empty()) {
		m_tmr_wheel.push_front(newTimer);
		std::cout << "INFO: TmrWheel::insert to empty wheel newTimer expire time is:" << newTimer.first.count() << std::endl;
		m_cv.notify_all();
		return true;
	}
	if (m_tmr_wheel.front().first > newTimer.first) {
		m_tmr_wheel.front().first -= newTimer.first;
		m_tmr_wheel.push_front(newTimer);
		std::cout << "INFO: TmrWheel::insert in front of wheel newTimer expire time is:" << newTimer.first.count() << std::endl;
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
		std::cout << "INFO: TmrWheel::insert in middle of wheel newTimer expire time is:" << newTimer.first.count() << " after timer:" << itrprev->first.count() << std::endl;
		m_tmr_wheel.insert(itrprev,newTimer);
		m_cv.notify_all();
		return true;
	}

	return false;
}
