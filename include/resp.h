#ifndef _RESP_H
#define _RESP_H

#include <string>
#include <list>
#include <optional>
#include <variant>


class Serialize;
class Deserialize;


/*
*****************
* class Serialize
*****************
*/
class Serialize
{
public:

    Serialize() {}

    std::string simple_str(std::string& str);
    std::string err(std::string& str);
    std::string integer(int num);
    std::string bulk_str(std::string& str);
    std::string array(std::list<std::string>& str);

protected:
private:
};


/*
*****************
* class Deserialize
*****************
*/
class Deserialize
{
public:

	typedef std::variant<std::string
						,int
						,std::list<std::string>
						,std::list<std::pair<std::string,std::string>>> 
	DsrlzdVariant_t;

	Deserialize() {}

	bool simple_str(std::string str);
	bool err(std::string str);
	bool integer(std::string str);
	bool bulk_str(std::string str);
	std::optional<std::list<std::string>> array(std::string str);
	bool set(std::string str);

	std::string get_dsrlzed_str() {
		return std::get<std::string>(m_dsrlzed);
	}
	int get_dsrlzed_int() {
		return std::get<int>(m_dsrlzed);
	}
	std::list<std::string> get_dsrlzed_list() {
		return std::get<std::list<std::string>>(m_dsrlzed);
	}
	std::list<std::pair<std::string,std::string>> get_dsrlzed_pairlist() {
		return std::get<std::list<std::pair<std::string,std::string>>>(m_dsrlzed);
	}

protected:


	DsrlzdVariant_t	m_dsrlzed;

private:
};

/*
*****************
* class Resp
*****************
*/
class Resp 
{
public:

    enum DataType : char {SIMPLE_STR='+',ERR='-',INT=':',BULK_STR='$',ARR='*',SET='~'};

	Resp() {}

    static std::optional<std::string> serialize(DataType firstCh,std::string);
    static std::optional<std::string> serialize(DataType firstCh,int);
    static std::optional<std::string> serialize(DataType firstCh,int,std::string);
    static std::optional<std::string> serialize(DataType firstCh,std::list<std::string>);
    static std::optional<std::string> serialize(DataType firstCh,std::list<std::pair<std::string,std::string>>);

    static std::optional<Deserialize::DsrlzdVariant_t> deserialize(std::string str);
    static std::optional<std::string> deserialize_bulk(std::string str);
    static std::optional<std::list<std::string>> deserialize_arr(std::string str);

	static const std::string NULL_BULK; 
	static const std::string NULL_ARR;
    static const std::string DELIM;

protected:
private:

    static Serialize m_SRLZ;
    static Deserialize m_DSRLZ;



};
#endif
