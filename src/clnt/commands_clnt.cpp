
#include <string>
#include <array>
#include <vector>
#include <optional>

#include "commands_clnt.h"
#include "resp.h"
#include "logger.h"


/*
-- class CmdMgr
*/
std::optional<std::string> ClntCmdMgr::build_ping_request()
{
	std::string ping{std::string{m_CMD_TYP[CmdMgr::PING]}};
	std::optional<std::string> req=Resp::serialize(Resp::SIMPLE_STR,ping);
	if (!req) return std::nullopt;
	return req;
}
std::optional<std::string> ClntCmdMgr::build_hello_request()
{
	std::string hello{std::string{m_CMD_TYP[CmdMgr::HELLO]}};
	std::optional<std::string> req=Resp::serialize(Resp::SIMPLE_STR,hello);
	if (!req) return std::nullopt;
	return req;
}
std::optional<std::string> ClntCmdMgr::build_echo_request(std::string str)
{
	std::string echo{std::string{m_CMD_TYP[CmdMgr::ECHO]}};
	std::optional<std::string> req=Resp::serialize(Resp::ARR,{echo,str});
	if (!req) return std::nullopt;
	return req;
}
std::optional<std::string> ClntCmdMgr::build_set_request(std::pair<std::string,std::string>& kvpair)
{
	std::string setstr{std::string{m_CMD_TYP[CmdMgr::SET]}};
	std::optional<std::string> req=Resp::serialize(Resp::ARR,{setstr,kvpair.first,kvpair.second});
	if (!req) return std::nullopt;
	return req;
}
std::optional<std::string> ClntCmdMgr::build_get_request(std::string str)
{
	std::string getstr{std::string{m_CMD_TYP[CmdMgr::GET]}};
	std::optional<std::string> req=Resp::serialize(Resp::ARR,{getstr,str});
	if (!req) return std::nullopt;
	return req;
}
std::optional<std::string> ClntCmdMgr::process_response(char* str,int sz)
{
	std::optional<Deserialize::DsrlzdVariant_t> rspns=Resp::deserialize(std::string(str,sz));
	if (!rspns) {
		Logger::instance().log_error(" deserialize fail for str:" ,str); 
	}
	std::string rspnsstr{std::get<std::string>(*rspns)};
	return rspnsstr;
}
/*std::optional<std::string> ClntCmdMgr::process_set_response(char* str,int sz)
{
	std::optional<Deserialize::DsrlzdVariant_t> rspns=Resp::deserialize(std::string(str,sz));
	std::string rspnsstr{std::get<std::string>(*rspns)};
	std::cout << "INFO process_set_response rspns:" << rspnsstr << std::endl;
	return rspnsstr;
}*/

