#include <string>
#include <list>
#include <optional>
#include <iostream>

#include "resp.h"
#include "logger.h"


/*
-- class Resp
*/

Serialize Resp::m_SRLZ{};
Deserialize Resp::m_DSRLZ{};

const std::string Resp::NULL_BULK {"$-1\\r\\n"};
const std::string Resp::NULL_ARR {"*-1\\r\\n"};
const std::string Resp::DELIM {"\r\n"};
/*
*************************************
* Resp::serialize(DataType firstCh,std::string)
**************************************
*/
std::optional<std::string> Resp::serialize(Resp::DataType firstCh,std::string str)
{
    switch (firstCh) {
    case Resp::SIMPLE_STR:
        return m_SRLZ.simple_str(str);
    case Resp::ERR:
		return m_SRLZ.err(str);
	case BULK_STR:
		return m_SRLZ.bulk_str(str);
    default:
        break;
    };
    return std::nullopt;
}
/*
*************************************
* Resp::serialize(DataType firstCh,int)
**************************************
*/
std::optional<std::string> Resp::serialize(Resp::DataType firstCh,int num)
{
    switch (firstCh) {
    case Resp::INT:
        return m_SRLZ.integer(num);
	default:
		break;
	}
    return std::nullopt;
}
		
/*
*************************************
* Resp::serialize(DataType firstCh,int,std::string);
**************************************
*/
std::optional<std::string> Resp::serialize(Resp::DataType firstCh,int,std::string str)
{
    switch (firstCh) {
    case Resp::BULK_STR:
        return m_SRLZ.bulk_str(str);
    default:
        break;
    };
    return std::nullopt;
}

/*
*************************************
* Resp::serialize(DataType firstCh,std::list<std::string>);
**************************************
*/
std::optional<std::string> Resp::serialize(DataType firstCh,std::list<std::string> lstr)
{
    switch (firstCh) {
    case Resp::ARR:
		return m_SRLZ.array(lstr);
	default:
		break;
	}	
    return std::nullopt;
}

/*
*************************************
* Resp::serialize(DataType firstCh,std::list<std::pair<std::string>>);
**************************************
*/
std::optional<std::string> Resp::serialize(DataType /*firstCh*/,std::list<std::pair<std::string,std::string>> /*lpstr*/)
{
    return std::nullopt;
}

/*
*************************************
* Resp::deserialize(std::string& str) 
**************************************
*/
std::optional<std::string> Resp::deserialize_bulk(std::string str)
{
	if (str[0]!=Resp::BULK_STR) return std::nullopt;
	m_DSRLZ.bulk_str(str);
	return m_DSRLZ.get_dsrlzed_str();
}
std::optional<std::list<std::string>> Resp::deserialize_arr(std::string str)
{
	if (str[0]!=Resp::ARR) return std::nullopt;
	return m_DSRLZ.array(str);
	//return m_DSRLZ.get_dsrlzed_list();
}
std::optional<Deserialize::DsrlzdVariant_t> Resp::deserialize(std::string str)
{
    switch (str[0]) {
        case Resp::SIMPLE_STR:
            m_DSRLZ.simple_str(str);
			return m_DSRLZ.get_dsrlzed_str();
            break;
        case Resp::ERR:
            m_DSRLZ.err(str);
			return m_DSRLZ.get_dsrlzed_str();
            break;
        case Resp::INT:
            m_DSRLZ.integer(str);
			return m_DSRLZ.get_dsrlzed_int();
            break;
        case Resp::BULK_STR:
            m_DSRLZ.bulk_str(str);
			return m_DSRLZ.get_dsrlzed_str();
            break;
        case Resp::ARR:
            return m_DSRLZ.array(str);
			//return m_DSRLZ.get_dsrlzed_list();
            break;
        case Resp::SET:
            m_DSRLZ.set(str);
			return m_DSRLZ.get_dsrlzed_list();
            break;
        default:
			break;
   }
   return std::nullopt;
}

/*
-- class Serialize
*/
std::string Serialize::simple_str(std::string& str) 
{
	std::string simple {Resp::SIMPLE_STR};
    simple += str;
    simple += Resp::DELIM; 
    return simple;
}
std::string Serialize::integer(int num)
{
	std::string intstr {Resp::INT};
    intstr += std::to_string(num);
    intstr += Resp::DELIM; 
    return intstr;
}
std::string Serialize::err(std::string& str) 
{
	std::string err {Resp::ERR};
    err += str;
    err += Resp::DELIM; 
    return err;
}

std::string Serialize::bulk_str(std::string& str) 
{
    std::string bulk{Resp::BULK_STR};
    bulk += std::to_string(str.length());
    bulk += Resp::DELIM; 
    bulk += str;
    bulk += Resp::DELIM; 
	return bulk;
}
std::string Serialize::array(std::list<std::string>& lstr)
{
	std::string arrstr{Resp::ARR};
	arrstr += std::to_string(lstr.size());
	arrstr += Resp::DELIM;
	for (auto& currstr : lstr) {
		arrstr += Resp::BULK_STR;
		arrstr += std::to_string(currstr.length());
		arrstr += Resp::DELIM;
		arrstr += currstr;
		arrstr += Resp::DELIM;
	}
	return arrstr;
}
/*
-- class Deserialize
*/
 
/*
*************************************
* Deserialize::simple_str(std::string& str)
**************************************
*/
bool Deserialize::simple_str(std::string str)
{
	if (str[0] != Resp::SIMPLE_STR) 
		return false;
	std::string::size_type delimpos=str.find_first_of(Resp::DELIM);
	m_dsrlzed = str.substr(1,delimpos-1);
	Logger::instance().log_info(" Deserialize::simple_str " ,std::get<std::string>(m_dsrlzed));
    return true;
}

/*
*************************************
* Deserialize::err(std::string& str)
**************************************
*/
bool Deserialize::err(std::string str)
{
	if (str[0] != Resp::ERR) 
		return false;
	std::string::size_type delimpos=str.find_first_of(Resp::DELIM);
	m_dsrlzed = str.substr(1,delimpos-1);
	std::cout << std::get<std::string>(m_dsrlzed) << std::endl;
    return true;
}
/*
*************************************
* Deserialize::integer(std::string& str)
**************************************
*/
bool Deserialize::integer(std::string str)
{
	if (str[0]!=Resp::INT)
    	return false;
		
	std::string::size_type delimpos=str.find_first_of(Resp::DELIM);
	std::string integer = str.substr(1,delimpos-1);
	m_dsrlzed = std::stoi(integer);
	return true;
}
	
/*
*************************************
* Deserialize::bulk_str(std::string& str)
**************************************
*/
bool Deserialize::bulk_str(std::string str)
{
	if (str[0]!=Resp::BULK_STR)
		return false;
	std::string::size_type delimpos=str.find_first_of(Resp::DELIM);
	std::string len = str.substr(1,delimpos-1);
//	std::string::size_type delimpos1=str.find_first_of(Resp::DELIM,delimpos+Resp::DELIM.length());
	std::string cmd = str.substr(delimpos+Resp::DELIM.length(),std::stoi(len));
	m_dsrlzed = cmd;
    return true;
}
/*
*************************************
* Deserialize::array(std::string& str)
**************************************
*/
std::optional<std::list<std::string>> Deserialize::array(std::string str)
{
	if (str[0]!=Resp::ARR)
		return std::nullopt;
	std::string::size_type delimpos=str.find(Resp::DELIM);
	if (delimpos > str.length()) {
		Logger::instance().log_error(" Deserialize::array. str:" ,str ,"delimpos:" ,std::to_string(delimpos));
		return std::nullopt;
	}
	std::string arrsz = str.substr(1,delimpos-1);
	int iarrsz = std::stoi(arrsz);
	std::list<std::string> arrlst{};
	int pos=delimpos+Resp::DELIM.length();
	for (int i=0; i<iarrsz ; ++i) {
		if (!bulk_str(&str[pos])) return std::nullopt;
		std::string currbulk{std::get<std::string>(m_dsrlzed)};
		int currbulklen = currbulk.length();
		std::string lenstr = std::to_string(currbulklen);
		pos += currbulk.length()+Resp::DELIM.length() + 1 + lenstr.length()+Resp::DELIM.length();  
	 	arrlst.push_back(currbulk);	
	}
	m_dsrlzed = arrlst;
    return arrlst;
}

/*
*************************************
* Deserialize::set(std::string& str)
**************************************
*/
bool Deserialize::set(std::string )
{
    return false;
}

