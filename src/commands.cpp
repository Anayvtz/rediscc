
#include <string>
#include <array>
#include <vector>
#include <optional>

#include "commands.h"
#include "resp.h"


std::vector<std::string> CmdMgr::m_CMD_TYP { std::string("PING")
											, std::string("HELLO")
											, std::string("ECHO") 
											, std::string("SET") 
											, std::string("GET")
											, std::string("SETEX") };
/*
-- class CmdMgr
*/

/*
*	CmdMgr::find_cmd(char* str,int sz)
*/
CmdMgr::CmdType CmdMgr::find_cmd(char* str,int sz)
{
	std::string cmd2find(&str[1],sz-1);
	for (int i=0; auto cmd : m_CMD_TYP) {

		//if (cmd2find.starts_with(cmd))
		if (!cmd2find.compare(cmd))
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

		//if (cmd2find.starts_with(cmd))
		if (!cmd2find.compare(cmd))
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

		//if (cmd2find.starts_with(cmd))
		if (!cmd2find.compare(cmd))
			return 	static_cast<CmdType>(i);
		else ++i;
	}
	return CmdMgr::NOT_SUPPORTED;
}

