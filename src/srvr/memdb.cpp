
#include <iostream>
#include <string>
#include <optional>
#include <shared_mutex>
#include <set>
#include <map>
#include <unordered_map>
#include <memory>

#include "memdb.h"
#include "timer_wheel.h"
#include "logger.h"


MemDB::MemDB() : m_hash(),m_shrd_mtx(),m_tmr_wheel(new TmrWheel(this))
{
}
MemDB::~MemDB() noexcept
{
	delete m_tmr_wheel;
}
bool MemDB::insert(const std::string& key, const std::string& value)
{
	std::lock_guard<std::shared_mutex> wrtrLck(m_shrd_mtx);
	auto pairrc=m_hash.insert_or_assign(key,value);
	if (pairrc.second) {
		Logger::instance().log_info(" key:",key,"was INSERTED to hash with value: ",value);
	}
	else {
		Logger::instance().log_info(" key:",key,"was REASSIGNED to hash with value: ",value);
	}
	return true;
}
bool MemDB::insert(const std::string& key, const std::string& value,int expiresec)
{
	std::lock_guard<std::shared_mutex> wrtrLck(m_shrd_mtx);
	auto pairrc=m_hash.insert_or_assign(key,value);
	Logger::instance().log_info(" key:",key,"ABOUT TO INSERT to hash with value: ",value);
	Logger::instance().log_info(" expiresec is:", std::to_string(expiresec)); 
	m_tmr_wheel->insert(expiresec,pairrc.first);
	if (pairrc.second) {
		Logger::instance().log_info(" key:",key,"was INSERTED to hash with value: ",value);
	}
	else {
		Logger::instance().log_info(" key:",key,"was REASSIGNED to hash with value: ",value);
	}
	return true;
}
bool MemDB::insert(const std::string& key,const std::list<std::string>& lstval)
{
	std::lock_guard<std::shared_mutex> wrtrLck(m_shrd_mtx);
	auto pairrc=m_hash.insert_or_assign(key,lstval);
	if (pairrc.second) {
		Logger::instance().log_info(" key:" ,key ,"was INSERTED to hash with value: ");
		for (auto val: lstval) {
			Logger::instance().log_info(" list value:",val);
		}
	}
	else {
		Logger::instance().log_info(" key:",key ,"was REASSIGNED to hash with value: ");
		for (auto val: lstval) {
			Logger::instance().log_info(" list value:",val);
		}
	}
	return true;
}
bool MemDB::insert(const std::string& key,const std::set<std::string>& setval)
{
	std::lock_guard<std::shared_mutex> wrtrLck(m_shrd_mtx);
	auto pairrc=m_hash.insert_or_assign(key,setval);
	if (pairrc.second) {
		Logger::instance().log_info(" key:",key,"was INSERTED to hash with value: ");
		for (auto val : setval) {
			Logger::instance().log_info(" set val:",val);
		}
	}
	else {
		Logger::instance().log_info(" key:",key,"was REASSIGNED to hash with value: ");
		for (auto val : setval) {
			Logger::instance().log_info(" set val:", val);
		}
	}
	return true;
}
bool MemDB::insert(const std::string& key,const std::map<std::string,std::string>& mapval)
{
	std::lock_guard<std::shared_mutex> wrtrLck(m_shrd_mtx);
	auto pairrc=m_hash.insert_or_assign(key,mapval);
	if (pairrc.second) {
		Logger::instance().log_info(" key:",key,"was INSERTED to hash with map value: " );
		for (auto kv : mapval) {
			Logger::instance().log_info(" map key:",kv.first
										," map val:", kv.second);
		}
	}
	else {
		Logger::instance().log_info(" key:",key ,"was REASSIGNED to hash with value: " );
		for (auto kv : mapval) {
			Logger::instance().log_info(" map key:",kv.first
										," map val:" ,kv.second);
		}
	}
	return true;
}

bool MemDB::remove(const std::string& key)
{
	std::lock_guard<std::shared_mutex> wrtrLck(m_shrd_mtx);

	auto rc = m_hash.erase(key);
	if (!rc) {
		Logger::instance().log_error(" key:" ,key ,"was not removed" );
		return false;
	}

	return true;
}

std::optional<MemDB::ValueVariant_t> MemDB::retrieve(const std::string& key)
{
	std::shared_lock<std::shared_mutex> rdrdLck(m_shrd_mtx);
	auto found=m_hash.find(key);
	if (found==m_hash.end()) {
		Logger::instance().log_error(" key:", key, "was not found");
		return std::nullopt;
	}
	return found->second; 
}
