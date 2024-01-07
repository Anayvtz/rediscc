
#include <string>
#include <array>
#include <vector>
#include <optional>

#include "../include/commands.h"
#include "../include/resp.h"


std::vector<std::string> CmdMgr::m_CMD_TYP { std::string("PING")
											, std::string("HELLO")
											, std::string("ECHO") };
/*
-- class CmdMgr
*/
std::optional<std::string> CmdMgr::process_and_respond(char* str,int sz)
{
	CmdMgr::CmdType typ{CmdMgr::NOT_SUPPORTED};
	if ((typ=find_cmd(str,sz))==CmdMgr::NOT_SUPPORTED)
		if ((typ=find_cmd_bulk(str,sz))==CmdMgr::NOT_SUPPORTED)
			if ((typ=find_cmd_arr(str,sz))==CmdMgr::NOT_SUPPORTED) 
				return build_err_response();

	switch (typ) {
		case CmdMgr::PING:
			return build_pong_response();
		case CmdMgr::HELLO:
			return build_version_response();
		case CmdMgr::ECHO:
			return build_echo_response(str,sz);
		default:
			break;
	}
	return std::nullopt;
}

CmdMgr::CmdType CmdMgr::find_cmd(char* str,int sz)
{
	std::string cmd2find(&str[1],sz-1);
	for (int i=0; auto cmd : m_CMD_TYP) {

		if (cmd2find.starts_with(cmd))
			return 	static_cast<CmdType>(i);
		else ++i;
	}
	return CmdMgr::NOT_SUPPORTED;
}

CmdMgr::CmdType CmdMgr::find_cmd_bulk(char* str,int sz)
{
	std::string cmd2find(str,sz);
	std::optional<std::string>  rcbulk=Resp::deserialize_bulk(cmd2find);
	if (!rcbulk) return CmdMgr::NOT_SUPPORTED;
	cmd2find = *rcbulk;

	for (int i=0; auto cmd : m_CMD_TYP) {

		if (cmd2find.starts_with(cmd))
			return 	static_cast<CmdType>(i);
		else ++i;
	}
	return CmdMgr::NOT_SUPPORTED;
}
CmdMgr::CmdType CmdMgr::find_cmd_arr(char* str,int sz)
{
	std::string cmd2find(str,sz);
	std::optional<std::list<std::string>>  rcarr=Resp::deserialize_arr(cmd2find);
	if (!rcarr) return CmdMgr::NOT_SUPPORTED;
	cmd2find = (*rcarr).front();

	for (int i=0; auto cmd : m_CMD_TYP) {

		if (cmd2find.starts_with(cmd))
			return 	static_cast<CmdType>(i);
		else ++i;
	}
	return CmdMgr::NOT_SUPPORTED;
}

std::string CmdMgr::build_err_response() {
	std::string errstr{"Command Not Supported"};
	std::optional<std::string> srlzd=Resp::serialize(Resp::ERR,errstr);
	if (!srlzd) return "";
	return *srlzd;
}
std::string CmdMgr::build_pong_response() {
	std::string pong{"PONG"};
	std::optional<std::string> srlzd=Resp::serialize(Resp::SIMPLE_STR,pong);
	if (!srlzd) return "";
	return *srlzd;
}
std::optional<std::string> CmdMgr::process_pong_response(char* str,int sz)
{
	std::optional<Deserialize::DsrlzdVariant_t> rspns=Resp::deserialize(std::string(str,sz));
	std::string rspnsstr{std::get<std::string>(*rspns)};
	return rspnsstr;
}
std::string CmdMgr::build_version_response()
{
	return "";
}
std::optional<std::string> CmdMgr::build_echo_response(char* str,int sz)
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
	
std::optional<std::string> CmdMgr::build_ping_request()
{
	std::string ping{std::string{m_CMD_TYP[CmdMgr::PING]}};
	std::optional<std::string> req=Resp::serialize(Resp::SIMPLE_STR,ping);
	if (!req) return std::nullopt;
	return req;
}
void CmdMgr::build_hello_request()
{
}
void build_echo_request(std::string )
{
}
