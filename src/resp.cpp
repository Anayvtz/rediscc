#include <string>
#include <list>
#include <optional>

#include "../include/resp.h"


/*
-- class Resp
*/

Serialize Resp::SRLZ{};
Deserialize Resp::DSRLZ{};
/*
*************************************
* Resp::serialize(DataType firstCh,std::string)
**************************************
*/
std::optional<std::string> Resp::serialize(Resp::DataType firstCh,std::string str)
{
    switch (firstCh) {
    case Resp::SIMPLE_STR:
        return SRLZ.simple_str(str);
    case Resp::ERR:
		return SRLZ.err(str);
    default:
        break;
    };
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
        return SRLZ.bulk_str(str);
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
		return SRLZ.array(lstr);
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
std::optional<std::string> Resp::serialize(DataType firstCh,std::list<std::pair<std::string,std::string>> lpstr)
{
    return std::nullopt;
}

/*
*************************************
* Resp::deserialize(std::string& str) 
**************************************
*/
bool Resp::deserialize(std::string& str) 
{
    switch (str[0]) {
        case Resp::SIMPLE_STR:
            DSRLZ.simple_str(str);
            break;
        case Resp::ERR:
            DSRLZ.err(str);
            break;
        case Resp::INT:
            DSRLZ.integer(str);
            break;
        case Resp::BULK_STR:
            DSRLZ.bulk_str(str);
            break;
        case Resp::ARR:
            DSRLZ.array(str);
            break;
        case Resp::SET:
            DSRLZ.set(str);
            break;
        default:
            return false;
   }
   return true;
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
bool Deserialize::simple_str(std::string& str)
{
    return false;
}

/*
*************************************
* Deserialize::err(std::string& str)
**************************************
*/
bool Deserialize::err(std::string& str)
{
    return false;
}
/*
*************************************
* Deserialize::integer(std::string& str)
**************************************
*/
bool Deserialize::integer(std::string& str)
{
    return false;
}
/*
*************************************
* Deserialize::bulk_str(std::string& str)
**************************************
*/
bool Deserialize::bulk_str(std::string& str)
{
    return false;
}
/*
*************************************
* Deserialize::array(std::string& str)
**************************************
*/
bool Deserialize::array(std::string& str)
{
    return false;
}

/*
*************************************
* Deserialize::set(std::string& str)
**************************************
*/
bool Deserialize::set(std::string& str)
{
    return false;
}

