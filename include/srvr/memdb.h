#ifndef _MEMDB_H
#define _MEMDB_H

#include <string>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <variant>
#include <shared_mutex>
#include <memory>
#include <optional>

class TmrWheel;

class MemDB
{
public:

	using ValueVariant_t = std::variant<std::string
									,std::list<std::string>
									,std::set<std::string>
									,std::map<std::string,std::string>>;	
	using Hash_t = std::unordered_map<std::string,ValueVariant_t>;

	MemDB();
	~MemDB () noexcept ;

	bool insert(const std::string&,const std::string&);
	bool insert(const std::string&,const std::string&,int);
	bool insert(const std::string&,const std::list<std::string>&);
	bool insert(const std::string&,const std::set<std::string>&);
	bool insert(const std::string&,const std::map<std::string,std::string>&);

	bool remove(const std::string&);

	std::optional<ValueVariant_t> retrieve(const std::string&);

protected:

	Hash_t m_hash;
	std::shared_mutex	m_shrd_mtx;
	//std::unique_ptr<TmrWheel>	m_tmr_wheel;
	TmrWheel*	m_tmr_wheel;
	

private:
};

#endif
