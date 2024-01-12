
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
		std::cout<<"INFO: key:" << key << "was INSERTED to hash with value: " << value << std::endl;
	}
	else {
		std::cout<<"INFO: key:" << key << "was REASSIGNED to hash with value: " << value << std::endl;
	}
	return true;
}
bool MemDB::insert(const std::string& key, const std::string& value,int expiresec)
{
	std::lock_guard<std::shared_mutex> wrtrLck(m_shrd_mtx);
	auto pairrc=m_hash.insert_or_assign(key,value);
	std::cout<<"INFO: key:" << key << "ABOUT TO INSERT to hash with value: " << value << " expiresec is:" << expiresec << std::endl;
	m_tmr_wheel->insert(expiresec,pairrc.first);
	if (pairrc.second) {
		std::cout<<"INFO: key:" << key << "was INSERTED to hash with value: " << value << std::endl;
	}
	else {
		std::cout<<"INFO: key:" << key << "was REASSIGNED to hash with value: " << value << std::endl;
	}
	return true;
}
bool MemDB::insert(const std::string& key,const std::list<std::string>& lstval)
{
	std::lock_guard<std::shared_mutex> wrtrLck(m_shrd_mtx);
	auto pairrc=m_hash.insert_or_assign(key,lstval);
	if (pairrc.second) {
		std::cout<<"INFO: key:" << key << "was INSERTED to hash with value: " << std::endl;
		for (auto val: lstval) {
			std::cout << " list value:" << val << std::endl;
		}
	}
	else {
		std::cout<<"INFO: key:" << key << "was REASSIGNED to hash with value: " << std::endl;
		for (auto val: lstval) {
			std::cout << " list value:" << val << std::endl;
		}
	}
	return true;
}
bool MemDB::insert(const std::string& key,const std::set<std::string>& setval)
{
	std::lock_guard<std::shared_mutex> wrtrLck(m_shrd_mtx);
	auto pairrc=m_hash.insert_or_assign(key,setval);
	if (pairrc.second) {
		std::cout<<"INFO: key:" << key << "was INSERTED to hash with value: " <<std::endl;
		for (auto val : setval) {
			std::cout << " set val:" << val << std::endl;
		}
	}
	else {
		std::cout<<"INFO: key:" << key << "was REASSIGNED to hash with value: " << std::endl;
		for (auto val : setval) {
			std::cout << " set val:" << val << std::endl;
		}
	}
	return true;
}
bool MemDB::insert(const std::string& key,const std::map<std::string,std::string>& mapval)
{
	std::lock_guard<std::shared_mutex> wrtrLck(m_shrd_mtx);
	auto pairrc=m_hash.insert_or_assign(key,mapval);
	if (pairrc.second) {
		std::cout<<"INFO: key:" << key << "was INSERTED to hash with map value: " << std::endl;
		for (auto kv : mapval) {
			std::cout << " map key:" << kv.first
						<< " map val:" << kv.second
						<< std::endl;
		}
	}
	else {
		std::cout<<"INFO: key:" << key << "was REASSIGNED to hash with value: "  << std::endl;
		for (auto kv : mapval) {
			std::cout << " map key:" << kv.first
						<< " map val:" << kv.second
						<< std::endl;
		}
	}
	return true;
}

bool MemDB::remove(const std::string& key)
{
	std::lock_guard<std::shared_mutex> wrtrLck(m_shrd_mtx);

	auto rc = m_hash.erase(key);
	if (!rc) {
		std::cout << "ERR: key:" << key << "was not removed" << std::endl;
		return false;
	}

	return true;
}

std::optional<MemDB::ValueVariant_t> MemDB::retrieve(const std::string& key)
{
	std::shared_lock<std::shared_mutex> rdrdLck(m_shrd_mtx);
	auto found=m_hash.find(key);
	if (found==m_hash.end()) {
		std::cout << "ERR: key:" << key << "was not found" << std::endl;
		return std::nullopt;
	}
	return found->second; 
}
