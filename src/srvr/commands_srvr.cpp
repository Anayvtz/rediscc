
#include <string>
#include <array>
#include <vector>
#include <optional>
#include <climits>

#include "commands_srvr.h"
#include "resp.h"
#include "srvr.h"


/*
-- class CmdMgr
*/
std::optional<std::string> SrvrCmdMgr::process_and_respond(char* str,int sz,RedisSrvr& redis)
{
	CmdMgr::CmdType typ{CmdMgr::NOT_SUPPORTED};
	if ((typ=find_cmd(str,sz))==CmdMgr::NOT_SUPPORTED)
		if ((typ=find_cmd_bulk(str,sz))==CmdMgr::NOT_SUPPORTED)
			if ((typ=find_cmd_arr(str,sz))==CmdMgr::NOT_SUPPORTED) 
				return build_err_response("COMMAND not supported");

	switch (typ) {
		case CmdMgr::PING:
			return build_pong_response();
		case CmdMgr::HELLO:
			return build_version_response();
		case CmdMgr::ECHO:
			return build_echo_response(str,sz);
		case CmdMgr::SET:
			return build_set_response(str,sz,redis);
		case CmdMgr::SETEX:
			return build_setex_response(str,sz,redis);
		case CmdMgr::GET:
			return build_get_response(str,sz,redis);
		default:
			break;
	}
	return std::nullopt;
}

std::optional<std::string> SrvrCmdMgr::build_err_response(std::string errstr) 
{
	//std::string errstr{"Command Not Supported"};
	std::optional<std::string> srlzd=Resp::serialize(Resp::ERR,errstr);
	if (!srlzd) return "";
	return *srlzd;
}
std::string SrvrCmdMgr::build_ok_response() {
	std::string okstr{"OK"};
	std::optional<std::string> srlzd=Resp::serialize(Resp::SIMPLE_STR,okstr);
	if (!srlzd) return "";
	return *srlzd;
}
std::string SrvrCmdMgr::build_pong_response() {
	std::string pong{"PONG"};
	std::optional<std::string> srlzd=Resp::serialize(Resp::SIMPLE_STR,pong);
	if (!srlzd) return "";
	return *srlzd;
}
std::string SrvrCmdMgr::build_version_response()
{
	return "";
}
std::optional<std::string> SrvrCmdMgr::build_echo_response(char* str,int sz)
{
	std::optional<Deserialize::DsrlzdVariant_t> dsrlzd = std::nullopt;
	dsrlzd = Resp::deserialize(std::string{str
								,static_cast<std::string::size_type>(sz)});
	if (!dsrlzd) return std::nullopt;
	std::string respstr=std::get<std::string>(*dsrlzd);
	std::optional<std::string> echostr = Resp::serialize(Resp::SIMPLE_STR,respstr);
	if (echostr)
		std::cout << "INFO: echo str is:" << *echostr << std::endl;
	else {
		std::cout << "ERR: serialize echostr failed" << std::endl;
	}
	return echostr;
}

std::optional<std::string> SrvrCmdMgr::build_set_response(char* str,int sz,RedisSrvr& rediss)
{
	std::optional<std::list<std::string>> lstreq=Resp::deserialize_arr(std::string{str,static_cast<std::string::size_type>(sz)});
	if (!lstreq) return std::nullopt;
	(*lstreq).pop_front();
	std::string key=(*lstreq).front();
	std::string value=(*lstreq).back();
	rediss.insert2memdb(key,value);
	return build_ok_response();
}
std::optional<std::string> SrvrCmdMgr::build_setex_response(char* str,int sz,RedisSrvr& rediss)
{
	std::optional<std::list<std::string>> lstreq=Resp::deserialize_arr(std::string{str,static_cast<std::string::size_type>(sz)});
	if (!lstreq) return std::nullopt;
	(*lstreq).pop_front();
	std::string key=(*lstreq).front();
	(*lstreq).pop_front();
	std::string expiresec=(*lstreq).front();
	std::string max_int = std::to_string(INT_MAX);
	if (expiresec.length() > max_int.length())
		expiresec = max_int;
	else if (expiresec.length()==max_int.length()) {
		expiresec=max_int;
	}
		
	std::string value=(*lstreq).back();
	int expireseci = std::stoi(expiresec);
	if (expireseci < 0) {
		expireseci = INT_MAX;
	}
	if (!rediss.insert2memdb(key,value,expireseci)) {
		return build_err_response("Command was not inserted to memDB");
	}
	return build_ok_response();
}
std::optional<std::string> SrvrCmdMgr::build_get_response(char* str,int sz,RedisSrvr& rediss)
{
	std::optional<std::list<std::string>> lstreq=Resp::deserialize_arr(std::string{str,static_cast<std::string::size_type>(sz)});
	if (!lstreq) { std::cout << "ERR: build_get_response  deserialize_arr failed for str:" << str << std::endl;
		return std::nullopt;
	}
	(*lstreq).pop_front();
	std::string key=(*lstreq).front();
	std::optional<MemDB::ValueVariant_t> val=rediss.retrieve_from_memdb(key);
	if (!val) {
		std::cout << "ERR: build_get_response  retrv from memDB failed for str:" << str << std::endl;
		std::optional<std::string> errstr=build_err_response("GET failed.could not find key");
		return errstr;
	}
	if (std::holds_alternative<std::string>(*val)) {
		std::cout << "INFO: build_get_response from str:" << str << " rtrv string:" << std::get<std::string>(*val) << std::endl;
		std::optional<std::string> getrspns=Resp::serialize(Resp::SIMPLE_STR,std::get<std::string>(*val));
		if (!getrspns) {
			std::cout << "ERR: build_get_response serialize simple_str response" << std::endl;
			return std::nullopt;
		}
		return (*getrspns);
	}
	if (std::holds_alternative<std::list<std::string>>(*val)) {
		std::cout << "INFO: build_get_response . from str:" << str << "retrv list" << std::endl;
		std::optional<std::string> srlzd=Resp::serialize(Resp::ARR,std::get<std::list<std::string>>(*val));
		if (!srlzd) {
			std::cout << "ERR: build_get_response serialize arr response" << std::endl;
			return std::nullopt;
		}
		return *srlzd;
	}
	return std::nullopt;
}

	
