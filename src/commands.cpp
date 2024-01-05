
#include <string>
#include <array>
#include <optional>

#include "../include/commands.h"


enum CmdType { PING,HELLO,ECHO };
std::array<std::string,3> m_CMD_TYP { "PING", "HELLO", "ECHO" };
/*
--
*/
std::optional<std::string> CmdMgr::process_response(char* ,int )
{
	return std::nullopt;
}
