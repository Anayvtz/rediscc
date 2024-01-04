
#include <string>
#include <list>
#include <optional>


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

      Deserialize() {}

      bool simple_str(std::string& str);
      bool err(std::string& str);
      bool integer(std::string& str);
      bool bulk_str(std::string& str);
      bool array(std::string& str);
      bool set(std::string& str);

protected:
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

//	friend class Serialize;
//	friend class Deserialize;

    enum DataType : char {SIMPLE_STR='+',ERR='-',INT=':',BULK_STR='$',ARR='*',SET='~'};

	Resp() {}

    static std::optional<std::string> serialize(DataType firstCh,std::string);
    static std::optional<std::string> serialize(DataType firstCh,int,std::string);
    static std::optional<std::string> serialize(DataType firstCh,std::list<std::string>);
    static std::optional<std::string> serialize(DataType firstCh,std::list<std::pair<std::string,std::string>>);

    static bool deserialize(std::string& str);

	static constexpr const char* NULL_BULK {"$-1\\r\\n"};
	static constexpr const char* NULL_ARR {"*-1\\r\\n"};
    static constexpr const char* DELIM {"\\r\\n"};

protected:
private:

    static Serialize SRLZ;
    static Deserialize DSRLZ;



};
