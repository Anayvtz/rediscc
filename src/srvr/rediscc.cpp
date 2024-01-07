
#include <cstdio>
#include <string>
#include <iostream>

#include "../../include/rediscc.h"
#include "../../include/resp.h"
#include "../../include/srvr.h"

int main()
{

	Resp resp;

	std::string str{"hello"};
	std::optional<std::string> simple=resp.serialize(Resp::SIMPLE_STR,str);
	if (simple) {
		std::cout << *simple << std::endl;
	}

	std::optional<Deserialize::DsrlzdVariant_t> rcsimple=resp.deserialize(*simple);
	if (!rcsimple) std::cout << *simple << " not desrialized" << std::endl; 
	else std::cout << "deserialized string is:" << std::get<std::string>(*rcsimple) << std::endl;

	std::optional<std::string> errstr=resp.serialize(Resp::ERR
													,std::string("Error Message"));
	if (errstr) {
		std::cout << *errstr << std::endl;
	}

	std::optional<Deserialize::DsrlzdVariant_t>  rcerr=resp.deserialize(*errstr);
	if (!rcerr) std::cout << *errstr << " not desrialized" << std::endl; 
	else std::cout << "deserialized string is:" << std::get<std::string>(*rcerr) << std::endl;

	std::optional<std::string> bulk=resp.serialize(Resp::BULK_STR,str.length(),str);
	if (bulk) {
		std::cout << *bulk << std::endl;
	}
	std::optional<Deserialize::DsrlzdVariant_t>  rcbulk=resp.deserialize(*bulk);
	if (!rcbulk) std::cout << *bulk << " not desrialized" << std::endl; 
	else std::cout << "deserialized string is:" << std::get<std::string>(*rcbulk) << std::endl;


	std::optional<std::string> bulk2=resp.serialize(Resp::BULK_STR,0,"");
	if (bulk2) {
		std::cout << *bulk2 << std::endl;
	}
	std::optional<Deserialize::DsrlzdVariant_t>  rcbulk2=resp.deserialize(*bulk2);
	if (!rcbulk2) std::cout << *bulk2 << " not desrialized" << std::endl; 
	else std::cout << "deserialized string is:" << std::get<std::string>(*rcbulk2) << std::endl;

	std::list<std::string> lstr{"shame","on","you"};
	std::optional<std::string> arrstr=resp.serialize(Resp::ARR,lstr);
	if (arrstr) {
		std::cout << *arrstr << std::endl;
	}
	std::optional<Deserialize::DsrlzdVariant_t>  rcarr=resp.deserialize(*arrstr);
	if (!rcarr) std::cout << *arrstr << " not desrialized" << std::endl; 
	else {std::cout 	<< "deserialized string is:" 
					<< std::endl;
		for (auto currstr : std::get<std::list<std::string>>(*rcarr) ) {
			std::cout << "array element:" << currstr << std::endl;
		}	
	}

	int num=123;
	std::optional<std::string> intstr=resp.serialize(Resp::INT,num);
	if (intstr) {
		std::cout << "integer is:" << *intstr << std::endl;
	}
	

	RedisSrvr srvr;
	srvr.activate();

	return 0;
}

