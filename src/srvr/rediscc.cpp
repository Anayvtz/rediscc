
#include <cstdio>
#include <string>
#include <iostream>

#include "../../include/rediscc.h"
#include "../../include/resp.h"

int main()
{

	Resp resp;
	std::string str{"hello"};
	std::optional<std::string> simple=resp.serialize(Resp::SIMPLE_STR,str);
	if (simple) {
		std::cout << *simple << std::endl;
	}
	std::optional<std::string> errstr=resp.serialize(Resp::ERR,std::string("Error Message"));
	if (errstr) {
		std::cout << *errstr << std::endl;
	}
	std::optional<std::string> bulk=resp.serialize(Resp::BULK_STR,str.length(),str);
	if (bulk) {
		std::cout << *bulk << std::endl;
	}
	std::list<std::string> lstr{"shame","on","you"};
	std::optional<std::string> arrstr=resp.serialize(Resp::ARR,lstr);
	if (arrstr) {
		std::cout << *arrstr << std::endl;
	}
	return 0;
}

